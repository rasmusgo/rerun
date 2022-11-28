use ahash::{HashMap, HashMapExt};
use lazy_static::lazy_static;
use re_data_store::{FieldName, ObjPath};
use re_log_types::{
    context::{AnnotationInfo, ClassDescription, ClassId, KeypointId},
    DataPath, MsgId,
};
use std::{collections::BTreeMap, sync::Arc};

use crate::{misc::ViewerContext, ui::scene::SceneQuery};

#[derive(Clone, Debug)]
pub struct Annotations {
    pub msg_id: MsgId,
    pub classes: Vec<ClassDescription>,
}

impl Annotations {
    pub fn class_description(&self, class_id: Option<ClassId>) -> ResolvedClassDescription<'_> {
        ResolvedClassDescription(
            class_id.and_then(|class_id| self.classes.get(class_id.0 as usize)),
        )
    }
}

pub struct ResolvedClassDescription<'a>(pub Option<&'a ClassDescription>);

impl<'a> ResolvedClassDescription<'a> {
    pub fn annotation_info(&self) -> ResolvedAnnotationInfo {
        ResolvedAnnotationInfo(self.0.map(|desc| desc.info.clone()))
    }

    /// Merges class annotation info with keypoint annotation info (if existing respectively).
    pub fn annotation_info_with_keypoint(&self, keypoint_id: KeypointId) -> ResolvedAnnotationInfo {
        if let Some(desc) = self.0 {
            // Assuming that keypoint annotation is the rarer case, merging the entire annotation ahead of time
            // is cheaper than doing it lazily (which would cause more branches down the line for callsites without keypoints)
            if let Some(keypoint_annotation_info) = desc.keypoint_map.get(&keypoint_id) {
                ResolvedAnnotationInfo(Some(AnnotationInfo {
                    id: keypoint_id.0,
                    label: keypoint_annotation_info
                        .label
                        .clone()
                        .or_else(|| desc.info.label.clone()),
                    color: keypoint_annotation_info.color.or(desc.info.color),
                }))
            } else {
                self.annotation_info()
            }
        } else {
            ResolvedAnnotationInfo(None)
        }
    }
}

#[derive(Clone, Copy)]
pub enum DefaultColor<'a> {
    OpaqueWhite,
    TransparentBlack,
    ObjPath(&'a ObjPath),
}

pub struct ResolvedAnnotationInfo(pub Option<AnnotationInfo>);

impl ResolvedAnnotationInfo {
    pub fn color(&self, color: Option<&[u8; 4]>, default_color: DefaultColor<'_>) -> [u8; 4] {
        if let Some(color) = color {
            *color
        } else if let Some(color) = self
            .0
            .as_ref()
            .and_then(|info| info.color.or_else(|| Some(auto_color(info.id))))
        {
            color
        } else {
            match default_color {
                DefaultColor::TransparentBlack => [0, 0, 0, 0],
                DefaultColor::OpaqueWhite => [255, 255, 255, 255],
                DefaultColor::ObjPath(obj_path) => {
                    auto_color((obj_path.hash64() % std::u16::MAX as u64) as u16)
                }
            }
        }
    }

    pub fn label(&self, label: Option<&String>) -> Option<String> {
        if let Some(label) = label {
            Some(label.clone())
        } else {
            self.0
                .as_ref()
                .and_then(|info| info.label.as_ref().map(ToString::to_string))
        }
    }
}

#[derive(Default, Clone, Debug)]
pub struct AnnotationMap(pub BTreeMap<ObjPath, Arc<Annotations>>);

impl AnnotationMap {
    pub(crate) fn load(&mut self, ctx: &mut ViewerContext<'_>, query: &SceneQuery<'_>) {
        crate::profile_function!();
        let field_name = FieldName::from("class_description");

        let mut found_annotations = HashMap::new();

        // Gather all class description batch ancestors for all visible object paths.
        for obj_path in query
            .obj_paths
            .iter()
            .filter(|obj_path| query.obj_props.get(obj_path).visible)
        {
            let mut next_parent = Some(obj_path.clone());
            while let Some(parent) = next_parent {
                // If we've visited this parent before it's safe to break early.
                // All of it's parents have have also been visited.
                if self.0.contains_key(&parent) {
                    break;
                }

                if let Some(field_store) = ctx
                    .log_db
                    .obj_db
                    .store
                    .get(&query.timeline)
                    .and_then(|timeline_store| timeline_store.get(&parent))
                    .and_then(|obj_store| obj_store.get(&field_name))
                {
                    // It's only an annotation list if we have a multi field of class descriptions
                    if let Ok(multi_field_store) =
                        field_store.get_multi::<re_log_types::ClassDescription>()
                    {
                        // TODO(andreas): Early ou on the first succesful query.
                        multi_field_store.query(
                            &query.time_query,
                            |_time, msg_id, batch_or_splat| {
                                if let re_data_store::BatchOrSplat::Batch(batch) = batch_or_splat {
                                    let annotations = found_annotations
                                        .entry(parent.clone())
                                        .or_insert_with(|| {
                                            Arc::new(Annotations {
                                                msg_id: *msg_id,
                                                classes: batch.values().cloned().collect(),
                                            })
                                        });
                                    self.0.insert(obj_path.clone(), annotations.clone());
                                }
                            },
                        );
                        break;
                    }
                }

                next_parent = parent.parent();
            }
        }
    }

    pub(crate) fn find_associated(
        ctx: &mut ViewerContext<'_>,
        mut obj_path: ObjPath,
    ) -> Option<(DataPath, Annotations)> {
        let timeline = ctx.rec_cfg.time_ctrl.timeline();
        let timeline_store = ctx.log_db.obj_db.store.get(timeline)?;
        let query_time = ctx.rec_cfg.time_ctrl.time()?.floor().as_i64();
        let field_name = FieldName::from("class_description");

        let annotation_context_for_path = |obj_path: &ObjPath| {
            let field_store = timeline_store.get(obj_path)?.get(&field_name)?;
            // it's only an annotation list if we have a multi field of class descriptions
            let multi_field_store = field_store
                .get_multi::<re_log_types::ClassDescription>()
                .ok()?;
            let (_, msg_id, re_data_store::BatchOrSplat::Batch(batch)) = multi_field_store.latest_at(&query_time)? else {
                return None;
            };

            Some((
                DataPath::new(obj_path.clone(), field_name),
                Annotations {
                    msg_id: *msg_id,
                    classes: batch.values().cloned().collect(),
                },
            ))
        };

        loop {
            obj_path = obj_path.parent()?;
            let annotations = annotation_context_for_path(&obj_path);
            if annotations.is_some() {
                return annotations;
            }
        }
    }

    // Search through the all prefixes of this object path until we find a
    // matching annotation. If we find nothing return the default `MISSING_ANNOTATIONS`.
    pub fn find<'a>(&self, obj_path: impl Into<&'a ObjPath>) -> Arc<Annotations> {
        let mut next_parent = Some(obj_path.into().clone());
        while let Some(parent) = next_parent {
            if let Some(legend) = self.0.get(&parent) {
                return legend.clone();
            }

            next_parent = parent.parent().clone();
        }

        // Otherwise return the missing legend
        Arc::clone(&MISSING_ANNOTATIONS)
    }
}

// ---

const MISSING_MSG_ID: MsgId = MsgId::ZERO;

lazy_static! {
    static ref MISSING_ANNOTATIONS: Arc<Annotations> = {
        Arc::new(Annotations {
            msg_id: MISSING_MSG_ID,
            classes: Vec::new(),
        })
    };
}

// default colors
// Borrowed from `egui::PlotUi`
pub fn auto_color(val: u16) -> [u8; 4] {
    let golden_ratio = (5.0_f32.sqrt() - 1.0) / 2.0; // 0.61803398875
    let h = val as f32 * golden_ratio;
    let color = egui::Color32::from(egui::color::Hsva::new(h, 0.85, 0.5, 1.0));
    color.to_array()
}
