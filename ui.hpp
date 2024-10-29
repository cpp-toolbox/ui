#ifndef UI_HPP
#define UI_HPP

#include <functional>
#include "sbpt_generated_includes.hpp"

struct UIRect {
    IVPSolidColor ivpsc;
    bool mouse_above;
};

struct UITextBox {
    IVPSolidColor background_ivpsc;
    IVPTextured text_drawing_data;
    bool mouse_above;
};

struct UIClickableTextBox {
    std::function<void()> on_click;
    std::function<void()> on_hover;
    IVPSolidColor ivpsc;
    IVPTextured text_drawing_data;
    glm::vec3 regular_color;
    glm::vec3 hover_color;
    Rectangle rect;
    bool mouse_inside;
};

struct UIInputBox {
    std::function<void(std::string)> on_confirm;
    IVPSolidColor background_ivpsc;
    IVPTextured text_drawing_data;
    std::string placeholder_text;
    std::string contents;
    glm::vec3 regular_color;
    glm::vec3 focused_color;
    Rectangle rect;
    bool focused;
};

class UI {
  public:
    UI(const FontAtlas &font_atlas) : font_atlas(font_atlas) {};

    void process_mouse_position(const glm::vec2 &mouse_pos_ndc);
    void process_mouse_just_clicked(const glm::vec2 &mouse_pos_ndc);
    void process_key_press(const std::string &character_pressed);
    void process_confirm_action();
    void process_delete_action();

    void add_colored_rectangle(const glm::vec3 &draw_pos_ndc, float width, float height,
                               const glm::vec3 &normalized_rgb);

    void add_colored_rectangle(float x_pos_ndc, float y_pos_ndc, float width, float height,
                               const glm::vec3 &normalized_rgb);

    /*void add_clickable_colored_rectangle(std::function<void()> on_click, float x_pos_ndc, float y_pos_ndc, float
     * width,*/
    /*                                     float height, const glm::vec3 &regular_color, const glm::vec3
     * &hover_color);*/

    void add_textbox(const std::string &text, float x_pos_ndc, float y_pos_ndc, float width, float height,
                     const glm::vec3 &normalized_rgb);

    /* why we pass our fucntions by reference:
     * Capturing State in Lambdas:
     * If the on_click function uses any external state or variables (like curr_state or other context-dependent
     * variables),  ensure that you capture them correctly. In the provided code, on_click is passed by value,
     * which means it will capture the current values of any external variables when the function is defined.
     * If those variables change later, the on_click function will still hold the old values unless you capture by
     reference
     * (e.g., by using [&] in a lambda). This is particularly relevant if curr_state needs to be updated when a text box
     is clicked.
     */
    void add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover,
                               const std::string &text, float x_pos_ndc, float y_pos_ndc, float width, float height,
                               const glm::vec3 &regular_color, const glm::vec3 &hover_color);

    void add_input_box(std::function<void(std::string)> &on_confirm, const std::string &placeholder_text,
                       float x_pos_ndc, float y_pos_ndc, float width, float height, const glm::vec3 &regular_color,
                       const glm::vec3 &focused_color);

    /*const std::vector<UIRect> &get_rectangles() const;*/
    /*const std::vector<UIClickableTextBox> &get_clickable_text_boxes() const;*/
    /*const std::vector<IVPTextured> &get_text_boxes() const;*/

    const std::vector<UIClickableTextBox> &get_clickable_text_boxes() const;
    const std::vector<UIInputBox> &get_input_boxes() const;
    const std::vector<UITextBox> &get_text_boxes() const;

  private:
    FontAtlas font_atlas;
    void disable_focus_on_all_input_boxes();

    std::vector<IVPTextured> drawable_text_information;

    std::vector<UIRect> rectangles;

    std::vector<UIClickableTextBox> clickable_text_boxes;
    std::vector<UITextBox> text_boxes;
    std::vector<UIInputBox> input_boxes;
};

#endif // UI_HPP
