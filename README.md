# ui
We need to turn this into a little bit like html, we need to add divs and ids so that we can grab oids of the objects and then do operations on them, because right now we have ways to get things, but no ways of getting them back

## usage
```cpp
 filesystem_browser.process_mouse_position(ndc_mouse_pos);

if (mouse_clicked_signal.is_just_on()) {
    filesystem_browser.process_mouse_just_clicked(ndc_mouse_pos);
}

for (auto &cb : filesystem_browser.get_colored_boxes()) {
    batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
        cb.id, cb.ivpsc.indices, cb.ivpsc.xyz_positions, cb.ivpsc.rgb_colors);
}

for (auto &tb : filesystem_browser.get_text_boxes()) {
    batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
        tb.id, tb.background_ivpsc.indices, tb.background_ivpsc.xyz_positions,
        tb.background_ivpsc.rgb_colors);

    batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
        tb.id, tb.text_drawing_data.indices, tb.text_drawing_data.xyz_positions,
        tb.text_drawing_data.texture_coordinates, tb.modified_signal.has_just_changed());
}

for (auto &tb : filesystem_browser.get_clickable_text_boxes()) {
    batcher.absolute_position_with_colored_vertex_shader_batcher.queue_draw(
        tb.id, tb.ivpsc.indices, tb.ivpsc.xyz_positions, tb.ivpsc.rgb_colors,
        tb.modified_signal.has_just_changed());

    batcher.transform_v_with_signed_distance_field_text_shader_batcher.queue_draw(
        tb.id, tb.text_drawing_data.indices, tb.text_drawing_data.xyz_positions,
        tb.text_drawing_data.texture_coordinates);
}
```
