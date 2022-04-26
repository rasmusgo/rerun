pub(crate) mod context_panel;
pub(crate) mod log_table_view;
pub(crate) mod space_view;
pub(crate) mod time_panel;
pub(crate) mod view2d;
pub(crate) mod view3d;

#[derive(Clone, Copy, Debug, PartialEq)]
pub(crate) enum Preview {
    Small,
    Medium,
    Specific(f32),
}
