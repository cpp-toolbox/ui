#include "ui.hpp"

bool is_point_in_rectangle(const vertex_geometry::Rectangle &rect, const glm::vec2 &point) {
    float half_width = rect.width / 2.0f;
    float half_height = rect.height / 2.0f;

    float left_bound = rect.center.x - half_width;
    float right_bound = rect.center.x + half_width;
    float bottom_bound = rect.center.y - half_height;
    float top_bound = rect.center.y + half_height;

    return (point.x >= left_bound && point.x <= right_bound && point.y >= bottom_bound && point.y <= top_bound);
}

void UI::process_mouse_position(const glm::vec2 &mouse_pos_ndc) {
    for (auto &cr : clickable_text_boxes) {
        if (is_point_in_rectangle(cr.rect, mouse_pos_ndc)) {
            if (not cr.mouse_inside) {
                cr.modified_signal.set_on();
                std::vector<glm::vec3> cs(cr.ivpsc.xyz_positions.size(), cr.hover_color);
                cr.ivpsc.rgb_colors = cs;
                cr.mouse_inside = true;
            }
        } else {
            if (cr.mouse_inside) {
                cr.modified_signal.set_off();
                std::vector<glm::vec3> cs(cr.ivpsc.xyz_positions.size(), cr.regular_color);
                cr.ivpsc.rgb_colors = cs;
                cr.mouse_inside = false;
            }
        }
    }
    for (auto &dd : dropdowns) {
        if (is_point_in_rectangle(dd.dropdown_rect, mouse_pos_ndc)) {
            if (not dd.mouse_inside) {
                dd.modified_signal.set_on();
                std::vector<glm::vec3> cs(dd.dropdown_background.xyz_positions.size(), dd.hover_color);
                dd.dropdown_background.rgb_colors = cs;
                dd.mouse_inside = true;
            }
        } else {
            if (dd.mouse_inside) {
                dd.modified_signal.set_off();
                std::vector<glm::vec3> cs(dd.dropdown_background.xyz_positions.size(), dd.regular_color);
                dd.dropdown_background.rgb_colors = cs;
                dd.mouse_inside = false;
            }
        }
    }
}

void UI::process_mouse_just_clicked(const glm::vec2 &mouse_pos_ndc) {

    bool already_clicked_a_text_box = false;
    for (auto &cr : clickable_text_boxes) {
        if (not already_clicked_a_text_box and is_point_in_rectangle(cr.rect, mouse_pos_ndc)) {
            cr.on_click();
            // we don't want to propagate clicks through to multiple.
            already_clicked_a_text_box = true;
        }
    }

    bool already_focused_something = false;
    for (auto &ib : input_boxes) {
        bool click_inside_box = is_point_in_rectangle(ib.rect, mouse_pos_ndc);
        if (not ib.focused) {
            if (not already_focused_something and click_inside_box) {
                std::cout << "clicked in input box" << std::endl;
                std::vector<glm::vec3> cs(ib.background_ivpsc.xyz_positions.size(), ib.focused_color);
                ib.background_ivpsc.rgb_colors = cs;
                // blank out the text box on click
                TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
                    ib.contents, ib.rect.center.x, ib.rect.center.y, ib.rect.width, ib.rect.height);
                draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
                ib.text_drawing_data = ivpt;
                ib.focused = true;
                ib.modified_signal.toggle_state();
                already_focused_something = true;
            }
        } else {
            if (not click_inside_box) {
                std::cout << "clicked out of input box" << std::endl;
                ib.focused = false;
                std::vector<glm::vec3> cs(ib.background_ivpsc.xyz_positions.size(), ib.regular_color);
                if (ib.contents.size() == 0) { // put back placeholder
                    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
                        ib.placeholder_text, ib.rect.center.x, ib.rect.center.y, ib.rect.width, ib.rect.height);
                    draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
                    ib.text_drawing_data = ivpt;
                }
                ib.modified_signal.toggle_state();
                ib.background_ivpsc.rgb_colors = cs;
            }
        }
    }

    already_focused_something = false;
    for (auto &dd : dropdowns) {
        bool click_inside_box = is_point_in_rectangle(dd.dropdown_rect, mouse_pos_ndc);
        if (not dd.dropdown_open) {
            if (not already_focused_something and click_inside_box) {
                std::cout << "clicked in dropdown" << std::endl;
                std::vector<glm::vec3> cs(dd.dropdown_background.xyz_positions.size(), dd.hover_color);
                dd.dropdown_background.rgb_colors = cs;
                // blank out the text box on click
                TextMesh tm = font_atlas.generate_text_mesh_size_constraints(dd.active_selection, dd.dropdown_rect);
                draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
                dd.dropdown_text_data = ivpt;
                dd.dropdown_open = true;
                dd.modified_signal.toggle_state();
                already_focused_something = true;
            }
        } else {
            if (not click_inside_box) {
                std::cout << "clicked out of input box" << std::endl;
                dd.dropdown_open = false;
                /*std::vector<glm::vec3> cs(dd.background_ivpsc.xyz_positions.size(), dd.regular_color);*/
                /*if (dd.contents.size() == 0) { // put back placeholder*/
                /*    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(*/
                /*        dd.placeholder_text, dd.rect.center.x, dd.rect.center.y, dd.rect.width, dd.rect.height);*/
                /*    IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);*/
                /*    dd.text_drawing_data = ivpt;*/
                /*}*/
                dd.modified_signal.toggle_state();
                /*dd.background_ivpsc.rgb_colors = cs;*/
            }
        }
    }
}

void UI::process_key_press(const std::string &character_pressed) {
    for (auto &input_box : input_boxes) {
        if (input_box.focused) {
            std::cout << "got here" << std::endl;
            input_box.contents += character_pressed;
            std::cout << input_box.contents << std::endl;
            TextMesh tm = font_atlas.generate_text_mesh_size_constraints(input_box.contents, input_box.rect.center.x,
                                                                         input_box.rect.center.y, input_box.rect.width,
                                                                         input_box.rect.height);
            draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
            input_box.text_drawing_data = ivpt;
            input_box.modified_signal.toggle_state();
            break; // only one thing ever focused.
        }
    }
}

void UI::process_confirm_action() {
    for (auto &input_box : input_boxes) {
        if (input_box.focused) {
            input_box.on_confirm(input_box.contents);

            // after hitting enter go back to regular non focused mode.
            input_box.focused = false;
            std::vector<glm::vec3> cs(input_box.background_ivpsc.xyz_positions.size(), input_box.regular_color);
            draw_info::IVPSolidColor ivpsc(input_box.background_ivpsc.indices, input_box.background_ivpsc.xyz_positions,
                                           cs);
            input_box.background_ivpsc = ivpsc;
            input_box.modified_signal.toggle_state();

            break;
        }
    }
}

void UI::process_delete_action() {
    for (auto &input_box : input_boxes) {
        if (input_box.focused) {
            if (input_box.contents.size() >= 1) {
                input_box.contents = input_box.contents.substr(0, input_box.contents.size() - 1);
            }
            std::string text_to_use = input_box.contents;
            if (input_box.contents.size() == 0) {
                text_to_use = input_box.placeholder_text;
            }
            TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text_to_use, input_box.rect.center.x,
                                                                         input_box.rect.center.y, input_box.rect.width,
                                                                         input_box.rect.height);
            draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
            input_box.text_drawing_data = ivpt;
            input_box.modified_signal.toggle_state();
            break; // only one thing ever focused.
        }
    }
}

void UI::add_colored_rectangle(vertex_geometry::Rectangle ndc_rectangle, const glm::vec3 &normalized_rgb) {
    this->add_colored_rectangle(ndc_rectangle.center.x, ndc_rectangle.center.y, ndc_rectangle.width,
                                ndc_rectangle.height, normalized_rgb);
}

void UI::add_colored_rectangle(float x_pos_ndc, float y_pos_ndc, float width, float height,
                               const glm::vec3 &normalized_rgb) {
    auto is = vertex_geometry::generate_rectangle_indices();
    auto vs = vertex_geometry::generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    int rect_id = abs_pos_object_id_generator.get_id();
    std::vector<glm::vec3> cs(vs.size(), normalized_rgb);
    draw_info::IVPSolidColor ivpsc(is, vs, cs, rect_id);
    rectangles.emplace_back(ivpsc);
}
/*void UI::add_clickable_colored_rectangle(std::function<void()> on_click, float x_pos_ndc, float y_pos_ndc, float
 * width,*/
/*                                         float height, const glm::vec3 &regular_color, const glm::vec3
 * &hover_color)
 * {*/
/*    auto is = generate_rectangle_indices();*/
/*    auto vs = generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);*/
/*    std::vector<glm::vec3> cs(vs.size(), regular_color);*/
/**/
/*    IVPSolidColor ivpsc(is, vs, cs);*/
/*    vertex_geometry::Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);*/
/**/
/*    clickable_text_boxes.emplace_back(on_click, ivpsc, regular_color, hover_color, rect, false);*/
/*};*/

int UI::add_textbox(const std::string &text, vertex_geometry::Rectangle ndc_text_rectangle,
                    const glm::vec3 &normalized_rgb) {
    return this->add_textbox(text, ndc_text_rectangle.center.x, ndc_text_rectangle.center.y, ndc_text_rectangle.width,
                             ndc_text_rectangle.height, normalized_rgb);
}

int UI::add_textbox(const std::string &text, float center_x_pos_ndc, float center_y_pos_ndc, float width, float height,
                    const glm::vec3 &normalized_rgb) {

    // this id is for grabbing an element from the UI object
    int textbox_id = ui_id_generator.get_id();
    // these are internal ones used to clean up UI elements when deleted.
    int rect_id = abs_pos_object_id_generator.get_id();
    int text_data_id = sdf_object_id_generator.get_id();

    auto is = vertex_geometry::generate_rectangle_indices();
    auto vs = vertex_geometry::generate_rectangle_vertices(center_x_pos_ndc, center_y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), normalized_rgb);
    draw_info::IVPSolidColor ivpsc(is, vs, cs, rect_id);
    // TODO: do we really need this, we're already storing this in two places!
    rectangles.emplace_back(ivpsc, textbox_id); // used here

    // NOTE: adding rectangles so that we can check for intersection easier
    glm::vec3 center(center_x_pos_ndc, center_y_pos_ndc, 0);
    vertex_geometry::Rectangle bounding_rect(center, width, height);
    TextMesh tm =
        font_atlas.generate_text_mesh_size_constraints(text, center_x_pos_ndc, center_y_pos_ndc, width, height);
    // TODO: think about why "" is ok as a texture here and then explain why when you know
    draw_info::IVPTextured text_ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates, "", text_data_id);

    UITextBox tb(ivpsc, text_ivpt, bounding_rect, textbox_id); // used here
    text_boxes.emplace_back(tb);

    return textbox_id;
};

void UI::modify_text_of_a_textbox(int doid, std::string new_text) {
    // Find the textbox with the given ID
    UITextBox *textbox = get_textbox(doid);

    if (textbox != nullptr) {
        // Modify the text mesh with the new text
        TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
            new_text, textbox->bounding_rect.center.x, textbox->bounding_rect.center.y, textbox->bounding_rect.width,
            textbox->bounding_rect.height);

        // NOTE: we re-use the drawing data id to avoid a ivp leak, else we'd have to delete and regenerate which we
        // won't do.
        textbox->text_drawing_data = draw_info::IVPTextured(tm.indices, tm.vertex_positions, tm.texture_coordinates, "",
                                                            textbox->text_drawing_data.id);
        textbox->modified_signal.toggle_state();
    }
}

void UI::modify_colored_rectangle(int doid, vertex_geometry::Rectangle ndc_rectangle) {
    // Find the textbox with the given ID
    UIRect *colored_rectangle = get_colored_rectangle(doid);

    if (colored_rectangle != nullptr) {
        // Modify the text mesh with the new text

        // indices don't have to change
        colored_rectangle->ivpsc.xyz_positions = ndc_rectangle.get_ivs().vertices;
        colored_rectangle->modified_signal.toggle_state();
    }
}

UITextBox *UI::get_textbox(int doid) {
    auto it = std::find_if(text_boxes.begin(), text_boxes.end(),
                           [doid](const UITextBox &obj) { return obj.parent_ui_id == doid; });

    if (it != text_boxes.end()) {
        return &(*it);
    }
    return nullptr;
}

UIRect *UI::get_colored_rectangle(int doid) {
    auto it = std::find_if(rectangles.begin(), rectangles.end(),
                           [doid](const UIRect &obj) { return obj.parent_ui_id == doid; });
    if (it != rectangles.end()) {
        return &(*it);
    }
    return nullptr;
}

int UI::add_dropdown(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                     const vertex_geometry::Rectangle &rect, const glm::vec3 &regular_color,
                     const glm::vec3 &hover_color, const std::vector<std::string> &options,
                     std::vector<std::function<void()>> option_on_clicks) {

    // main dropdown button
    auto ivs = rect.get_ivs();
    auto is = ivs.indices;
    auto vs = ivs.vertices;
    std::vector<glm::vec3> cs(vs.size(), regular_color);
    draw_info::IVPSolidColor ivpsc(is, vs, cs);
    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text, rect);
    draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
    // main dropdown button

    // now the dropdown buttons themselves
    std::vector<draw_info::IVPTextured> option_text_data;
    std::vector<draw_info::IVPSolidColor> option_background_rect_data;
    std::vector<vertex_geometry::Rectangle> dropdown_option_rects;
    int i = 1;
    for (const auto &option : options) {
        vertex_geometry::Rectangle option_rect = slide_rectangle(rect, 0, -i);
        dropdown_option_rects.push_back(option_rect);

        auto ivs = option_rect.get_ivs();
        auto is = ivs.indices;
        auto vs = ivs.vertices;

        std::vector<glm::vec3> cs(vs.size(), regular_color);
        draw_info::IVPSolidColor ivpsc(is, vs, cs);
        option_background_rect_data.push_back(ivpsc);

        TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text, option_rect);
        draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
        option_text_data.push_back(ivpt);

        i += 1;
    }
    UIDropdown dropdown(on_click, on_hover, ivpsc, ivpt, regular_color, hover_color, rect, options, option_on_clicks,
                        option_background_rect_data, option_text_data, dropdown_option_rects);
    dropdowns.emplace_back(dropdown);
    return dropdown.id;
}

// todo we don't need to take in a reference ot a rect to make our lives easier.
// in the future makt it take a const reference for the future.
int UI::add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                              vertex_geometry::Rectangle &rect, const glm::vec3 &regular_color,
                              const glm::vec3 &hover_color) {
    return this->add_clickable_textbox(on_click, on_hover, text, rect.center.x, rect.center.y, rect.width, rect.height,
                                       regular_color, hover_color);
}

UIClickableTextBox *UI::get_clickable_textbox(int do_id) {
    auto it = std::find_if(clickable_text_boxes.begin(), clickable_text_boxes.end(),
                           [do_id](const UIClickableTextBox &obj) { return obj.id == do_id; });

    if (it != clickable_text_boxes.end()) {
        return &(*it);
    }
    return nullptr;
}

bool UI::remove_clickable_textbox(int do_id) {
    auto it = std::find_if(clickable_text_boxes.begin(), clickable_text_boxes.end(),
                           [do_id](const UIClickableTextBox &obj) { return obj.id == do_id; });

    if (it != clickable_text_boxes.end()) {
        clickable_text_boxes.erase(it);
        return true; // Object was found and removed
    }
    return false; // Object not found
}

bool UI::remove_textbox(int do_id) {

    ui_id_generator.reclaim_id(do_id);

    auto text_it = std::find_if(text_boxes.begin(), text_boxes.end(),
                                [do_id](const UITextBox &obj) { return obj.parent_ui_id == do_id; });

    auto rect_it = std::find_if(rectangles.begin(), rectangles.end(),
                                [do_id](const UIRect &rect) { return rect.parent_ui_id == do_id; });

    bool removed = false;

    if (text_it != text_boxes.end()) {
        text_boxes.erase(text_it);
        removed = true;
    }

    if (rect_it != rectangles.end()) {
        rectangles.erase(rect_it);
        removed = true;
    }

    return removed;
}

int UI::add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                              float x_pos_ndc, float y_pos_ndc, float width, float height,
                              const glm::vec3 &regular_color, const glm::vec3 &hover_color) {
    auto is = vertex_geometry::generate_rectangle_indices();
    auto vs = vertex_geometry::generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), regular_color);

    draw_info::IVPSolidColor ivpsc(is, vs, cs);
    vertex_geometry::Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);

    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text, x_pos_ndc, y_pos_ndc, width, height);
    draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);

    UIClickableTextBox clickable_text_box(on_click, on_hover, ivpsc, ivpt, regular_color, hover_color, rect);
    clickable_text_boxes.emplace_back(clickable_text_box);
    return clickable_text_box.id;
};

void UI::add_clickable_textbox_copy_fun(std::function<void()> on_click, std::function<void()> on_hover,
                                        const std::string &text, vertex_geometry::Rectangle &rect,
                                        const glm::vec3 &regular_color, const glm::vec3 &hover_color) {
    this->add_clickable_textbox_copy_fun(on_click, on_hover, text, rect.center.x, rect.center.y, rect.width,
                                         rect.height, regular_color, hover_color);
}

void UI::add_clickable_textbox_copy_fun(std::function<void()> on_click, std::function<void()> on_hover,
                                        const std::string &text, float x_pos_ndc, float y_pos_ndc, float width,
                                        float height, const glm::vec3 &regular_color, const glm::vec3 &hover_color) {
    auto is = vertex_geometry::generate_rectangle_indices();
    auto vs = vertex_geometry::generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), regular_color);

    draw_info::IVPSolidColor ivpsc(is, vs, cs);
    vertex_geometry::Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);

    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text, x_pos_ndc, y_pos_ndc, width, height);
    draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);

    clickable_text_boxes.emplace_back(on_click, on_hover, ivpsc, ivpt, regular_color, hover_color, rect);
};

void UI::add_input_box(std::function<void(std::string)> &on_confirm, const std::string &placeholder_text,
                       const vertex_geometry::Rectangle &ndc_rect, const glm::vec3 &regular_color,
                       const glm::vec3 &focused_color) {
    return this->add_input_box(on_confirm, placeholder_text, ndc_rect.center.x, ndc_rect.center.y, ndc_rect.width,
                               ndc_rect.height, regular_color, focused_color);
}

void UI::add_input_box(std::function<void(std::string)> &on_confirm, const std::string &placeholder_text,
                       float x_pos_ndc, float y_pos_ndc, float width, float height, const glm::vec3 &regular_color,
                       const glm::vec3 &focused_color) {
    auto is = vertex_geometry::generate_rectangle_indices();
    auto vs = vertex_geometry::generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), regular_color);

    draw_info::IVPSolidColor ivpsc(is, vs, cs);
    vertex_geometry::Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);

    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(placeholder_text, x_pos_ndc, y_pos_ndc, width, height);
    draw_info::IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);

    input_boxes.emplace_back(on_confirm, ivpsc, ivpt, placeholder_text, "", regular_color, focused_color, rect);
};

const std::vector<UIClickableTextBox> &UI::get_clickable_text_boxes() const { return clickable_text_boxes; }
const std::vector<UIDropdown> &UI::get_dropdowns() const { return dropdowns; }
const std::vector<UIInputBox> &UI::get_input_boxes() const { return input_boxes; }
const std::vector<UITextBox> &UI::get_text_boxes() const { return text_boxes; }
const std::vector<UIRect> &UI::get_colored_boxes() const { return rectangles; }

void process_and_queue_render_ui(glm::vec2 ndc_mouse_pos, UI &curr_ui, IUIRenderSuite &ui_render_suite,
                                 const std::vector<std::string> &key_strings_just_pressed,
                                 bool delete_action_just_pressed, bool confirm_action_just_pressed,
                                 bool mouse_just_clicked) {

    curr_ui.process_mouse_position(ndc_mouse_pos);
    for (const auto &key_str : key_strings_just_pressed) {
        curr_ui.process_key_press(key_str);
    }

    if (delete_action_just_pressed) {
        curr_ui.process_delete_action();
    }

    if (confirm_action_just_pressed) {
        curr_ui.process_confirm_action();
    }

    if (mouse_just_clicked) {
        curr_ui.process_mouse_just_clicked(ndc_mouse_pos);
    }

    for (auto &cb : curr_ui.get_colored_boxes()) {
        ui_render_suite.render_colored_box(cb);
    }

    for (auto &tb : curr_ui.get_text_boxes()) {
        ui_render_suite.render_text_box(tb);
    }

    for (auto &cr : curr_ui.get_clickable_text_boxes()) {
        ui_render_suite.render_clickable_text_box(cr);
    }

    for (auto &ib : curr_ui.get_input_boxes()) {
        ui_render_suite.render_input_box(ib);
    }

    for (auto &dd : curr_ui.get_dropdowns()) {
        ui_render_suite.render_dropdown(dd);

        // render all the dropdown options if the parent dropdown is activated
        // TODO: needs to work better
        if (dd.dropdown_open) {
            int num_dropdowns = dd.dropdown_option_rects.size();
            for (int i = 0; i < num_dropdowns; i++) {
                draw_info::IVPSolidColor ivpsc = dd.dropdown_option_backgrounds[i];
                draw_info::IVPTextured ivpt = dd.dropdown_option_text_data[i];
                unsigned int doid = dd.dropdown_doids[i];

                ui_render_suite.render_dropdown_option(dd, ivpsc, ivpt, doid);
            }
        }
    }
}
