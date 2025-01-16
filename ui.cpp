#include "ui.hpp"

glm::vec2 UI::get_ndc_mouse_pos(unsigned int window_width, unsigned int window_height, double xpos, double ypos) {
    return {(2.0f * xpos) / window_width - 1.0f, 1.0f - (2.0f * ypos) / window_height};
}

bool is_point_in_rectangle(const Rectangle &rect, const glm::vec2 &point) {
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
}

void UI::process_mouse_just_clicked(const glm::vec2 &mouse_pos_ndc) {
    for (auto &cr : clickable_text_boxes) {
        if (is_point_in_rectangle(cr.rect, mouse_pos_ndc)) {
            cr.on_click();
        }
    }
    // clear focus on a click, if you click the same thing twice nothing happens.
    for (auto &input_box : input_boxes) {
        input_box.focused = false;
    }
    for (auto &ib : input_boxes) {
        if (is_point_in_rectangle(ib.rect, mouse_pos_ndc)) {
            std::vector<glm::vec3> cs(ib.background_ivpsc.xyz_positions.size(), ib.focused_color);
            ib.background_ivpsc.rgb_colors = cs;
            // blank out the text box on click
            TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
                ib.contents, ib.rect.center.x, ib.rect.center.y, ib.rect.width, ib.rect.height);
            IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
            ib.text_drawing_data = ivpt;
            ib.focused = true;
            break; // will only focus on the first thing if multiple overlap
        }
    }
    // go back to regular colors for unfocused thing
    for (auto &ib : input_boxes) {
        if (not ib.focused) {
            std::vector<glm::vec3> cs(ib.background_ivpsc.xyz_positions.size(), ib.regular_color);
            if (ib.contents.size() == 0) { // put back placeholder
                TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
                    ib.placeholder_text, ib.rect.center.x, ib.rect.center.y, ib.rect.width, ib.rect.height);
                IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
                ib.text_drawing_data = ivpt;
            }
            ib.background_ivpsc.rgb_colors = cs;
        }
    }
}

void UI::process_key_press(const std::string &character_pressed) {
    for (auto &input_box : input_boxes) {
        if (input_box.focused) {
            input_box.contents += character_pressed;
            TextMesh tm = font_atlas.generate_text_mesh_size_constraints(input_box.contents, input_box.rect.center.x,
                                                                         input_box.rect.center.y, input_box.rect.width,
                                                                         input_box.rect.height);
            IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
            input_box.text_drawing_data = ivpt;
            break; // only one thing ever focused.
        }
    }
}

void UI::process_confirm_action() {
    for (auto &input_box : input_boxes) {
        if (input_box.focused) {
            input_box.on_confirm(input_box.contents);
            TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
                input_box.placeholder_text, input_box.rect.center.x, input_box.rect.center.y, input_box.rect.width,
                input_box.rect.height);
            IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
            input_box.text_drawing_data = ivpt;

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
            IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
            input_box.text_drawing_data = ivpt;
            break; // only one thing ever focused.
        }
    }
}

void UI::add_colored_rectangle(Rectangle ndc_rectangle, const glm::vec3 &normalized_rgb) {
    this->add_colored_rectangle(ndc_rectangle.center.x, ndc_rectangle.center.y, ndc_rectangle.width,
                                ndc_rectangle.height, normalized_rgb);
}

void UI::add_colored_rectangle(float x_pos_ndc, float y_pos_ndc, float width, float height,
                               const glm::vec3 &normalized_rgb) {
    auto is = generate_rectangle_indices();
    auto vs = generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), normalized_rgb);
    IVPSolidColor ivpsc(is, vs, cs);
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
/*    Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);*/
/**/
/*    clickable_text_boxes.emplace_back(on_click, ivpsc, regular_color, hover_color, rect, false);*/
/*};*/

int UI::add_textbox(const std::string &text, Rectangle ndc_text_rectangle, const glm::vec3 &normalized_rgb) {
    return this->add_textbox(text, ndc_text_rectangle.center.x, ndc_text_rectangle.center.y, ndc_text_rectangle.width,
                             ndc_text_rectangle.height, normalized_rgb);
}

int UI::add_textbox(const std::string &text, float center_x_pos_ndc, float center_y_pos_ndc, float width, float height,
                    const glm::vec3 &normalized_rgb) {

    auto is = generate_rectangle_indices();
    auto vs = generate_rectangle_vertices(center_x_pos_ndc, center_y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), normalized_rgb);

    IVPSolidColor ivpsc(is, vs, cs);

    // adding rectangles so taht we can check for intersection easier.
    glm::vec3 center(center_x_pos_ndc, center_y_pos_ndc, 0);
    Rectangle bounding_rect(center, width, height);
    rectangles.emplace_back(ivpsc);
    TextMesh tm =
        font_atlas.generate_text_mesh_size_constraints(text, center_x_pos_ndc, center_y_pos_ndc, width, height);
    IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);
    UITextBox tb(ivpsc, ivpt, bounding_rect);
    text_boxes.emplace_back(tb);
    return tb.id;
};

void UI::modify_text_of_a_textbox(int doid, std::string new_text) {
    // Find the textbox with the given ID
    UITextBox *textbox = get_textbox(doid);

    if (textbox != nullptr) {
        // Modify the text mesh with the new text
        TextMesh tm = font_atlas.generate_text_mesh_size_constraints(
            new_text, textbox->bounding_rect.center.x, textbox->bounding_rect.center.y, textbox->bounding_rect.width,
            textbox->bounding_rect.height);

        textbox->text_drawing_data = IVPTextured(tm.indices, tm.vertex_positions, tm.texture_coordinates);
        textbox->modified_signal.toggle_state();
    }
}

UITextBox *UI::get_textbox(int doid) {
    auto it =
        std::find_if(text_boxes.begin(), text_boxes.end(), [doid](const UITextBox &obj) { return obj.id == doid; });

    if (it != text_boxes.end()) {
        return &(*it);
    }
    return nullptr;
}

int UI::add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                              Rectangle &rect, const glm::vec3 &regular_color, const glm::vec3 &hover_color) {
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
    auto it =
        std::find_if(text_boxes.begin(), text_boxes.end(), [do_id](const UITextBox &obj) { return obj.id == do_id; });

    if (it != text_boxes.end()) {
        text_boxes.erase(it);
        return true; // Object was found and removed
    }
    return false; // Object not found
}

int UI::add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                              float x_pos_ndc, float y_pos_ndc, float width, float height,
                              const glm::vec3 &regular_color, const glm::vec3 &hover_color) {
    auto is = generate_rectangle_indices();
    auto vs = generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), regular_color);

    IVPSolidColor ivpsc(is, vs, cs);
    Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);

    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text, x_pos_ndc, y_pos_ndc, width, height);
    IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);

    UIClickableTextBox clickable_text_box(on_click, on_hover, ivpsc, ivpt, regular_color, hover_color, rect);
    clickable_text_boxes.emplace_back(clickable_text_box);
    return clickable_text_box.id;
};

void UI::add_clickable_textbox_copy_fun(std::function<void()> on_click, std::function<void()> on_hover,
                                        const std::string &text, Rectangle &rect, const glm::vec3 &regular_color,
                                        const glm::vec3 &hover_color) {
    this->add_clickable_textbox_copy_fun(on_click, on_hover, text, rect.center.x, rect.center.y, rect.width,
                                         rect.height, regular_color, hover_color);
}

void UI::add_clickable_textbox_copy_fun(std::function<void()> on_click, std::function<void()> on_hover,
                                        const std::string &text, float x_pos_ndc, float y_pos_ndc, float width,
                                        float height, const glm::vec3 &regular_color, const glm::vec3 &hover_color) {
    auto is = generate_rectangle_indices();
    auto vs = generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), regular_color);

    IVPSolidColor ivpsc(is, vs, cs);
    Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);

    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(text, x_pos_ndc, y_pos_ndc, width, height);
    IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);

    clickable_text_boxes.emplace_back(on_click, on_hover, ivpsc, ivpt, regular_color, hover_color, rect);
};

void UI::add_input_box(std::function<void(std::string)> &on_confirm, const std::string &placeholder_text,
                       float x_pos_ndc, float y_pos_ndc, float width, float height, const glm::vec3 &regular_color,
                       const glm::vec3 &focused_color) {
    auto is = generate_rectangle_indices();
    auto vs = generate_rectangle_vertices(x_pos_ndc, y_pos_ndc, width, height);
    std::vector<glm::vec3> cs(vs.size(), regular_color);

    IVPSolidColor ivpsc(is, vs, cs);
    Rectangle rect(glm::vec3(x_pos_ndc, y_pos_ndc, 0), width, height);

    TextMesh tm = font_atlas.generate_text_mesh_size_constraints(placeholder_text, x_pos_ndc, y_pos_ndc, width, height);
    IVPTextured ivpt(tm.indices, tm.vertex_positions, tm.texture_coordinates);

    input_boxes.emplace_back(on_confirm, ivpsc, ivpt, placeholder_text, "", regular_color, focused_color, rect);
};

const std::vector<UIClickableTextBox> &UI::get_clickable_text_boxes() const { return clickable_text_boxes; }
const std::vector<UIInputBox> &UI::get_input_boxes() const { return input_boxes; }
const std::vector<UITextBox> &UI::get_text_boxes() const { return text_boxes; }
const std::vector<UIRect> &UI::get_colored_boxes() const { return rectangles; }
