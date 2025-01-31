# ui
We need to turn this into a little bit like html, we need to add divs and ids so that we can grab oids of the objects and then do operations on them, because right now we have ways to get things, but no ways of getting them back

## usage

Since rendering techniques may change, the ui class simply gives a way to export the geometry required to render the ui, in the `cpp toolbox` context then rendering and processing the ui can be put into a function like this:

```cpp
void process_and_queue_render_ui(glm::vec2 ndc_mouse_pos, UI &curr_ui, Batcher &batcher) {
    curr_ui.process_mouse_position(ndc_mouse_pos);

    if (mouse_just_clicked) {
        curr_ui.process_mouse_just_clicked(ndc_mouse_pos);
    }

    process_key_pressed_this_tick(curr_ui);

    for (auto &cb : curr_ui.get_colored_boxes()) {
        batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
            cb.id, cb.ivpsc.indices, cb.ivpsc.xyz_positions, cb.ivpsc.rgb_colors,
            cb.modified_signal.has_just_changed());
    }

    for (auto &tb : curr_ui.get_text_boxes()) {
        batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
            tb.id, tb.text_drawing_data.indices, tb.text_drawing_data.xyz_positions,
            tb.text_drawing_data.texture_coordinates);
        batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
            tb.id, tb.background_ivpsc.indices, tb.background_ivpsc.xyz_positions, tb.background_ivpsc.rgb_colors,
            tb.modified_signal.has_just_changed());
    }

    for (auto &cr : curr_ui.get_clickable_text_boxes()) {
        batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
            cr.id, cr.text_drawing_data.indices, cr.text_drawing_data.xyz_positions,
            cr.text_drawing_data.texture_coordinates);

        batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
            cr.id, cr.ivpsc.indices, cr.ivpsc.xyz_positions, cr.ivpsc.rgb_colors,
            cr.modified_signal.has_just_changed());
    }

    for (auto &ib : curr_ui.get_input_boxes()) {

        batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
            ib.id, ib.text_drawing_data.indices, ib.text_drawing_data.xyz_positions,
            ib.text_drawing_data.texture_coordinates, ib.modified_signal.has_just_changed());

        batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
            ib.id, ib.background_ivpsc.indices, ib.background_ivpsc.xyz_positions, ib.background_ivpsc.rgb_colors,
            ib.modified_signal.has_just_changed());
    }

    for (auto &dd : curr_ui.get_dropdowns()) {

        batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
            dd.id, dd.dropdown_text_data.indices, dd.dropdown_text_data.xyz_positions,
            dd.dropdown_text_data.texture_coordinates, dd.modified_signal.has_just_changed());

        batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
            dd.id, dd.dropdown_background.indices, dd.dropdown_background.xyz_positions,
            dd.dropdown_background.rgb_colors, dd.modified_signal.has_just_changed());

        // render all the dropdowns if they're active
        if (dd.dropdown_open) {
            int num_dropdowns = dd.dropdown_option_rects.size();
            for (int i = 0; i < num_dropdowns; i++) {
                IVPSolidColor ivpsc = dd.dropdown_option_backgrounds[i];
                IVPTextured ivpt = dd.dropdown_option_text_data[i];
                unsigned int doid = dd.dropdown_doids[i];

                batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
                    doid, ivpt.indices, ivpt.xyz_positions, ivpt.texture_coordinates,
                    dd.modified_signal.has_just_changed());

                batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
                    doid, ivpsc.indices, ivpsc.xyz_positions, ivpsc.rgb_colors, dd.modified_signal.has_just_changed());
            }
        }
    }
}
```
When rendering the ui, we usually want to ignore the 3d component of information so that it appears on top of the already rendered things, so before drawing you might want to disable the depth test:
```cpp
glDisable(GL_DEPTH_TEST);
batcher.absolute_position_with_colored_vertex_shader_batcher.draw_everything();
batcher.transform_v_with_signed_distance_field_text_shader_batcher.draw_everything();
glEnable(GL_DEPTH_TEST);
```
