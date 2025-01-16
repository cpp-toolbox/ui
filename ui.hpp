#ifndef UI_HPP
#define UI_HPP

#include <functional>
#include <optional>
#include "sbpt_generated_includes.hpp"

struct FileBrowser {
    float fsb_width;
    float fsb_height;
    float fsb_to_top_edge_dist;

    Rectangle background_rect;
    Rectangle current_directory_rect;
    Rectangle main_file_view_rect;
    Rectangle file_selection_bar;
    Rectangle open_button;
    Rectangle close_button;
    Rectangle up_a_dir_button;

    // Constructor to initialize the struct with the necessary parameters
    FileBrowser(float width, float height)
        : fsb_width(width), fsb_height(height), fsb_to_top_edge_dist(height / 2.0),
          background_rect(create_rectangle(0, 0, width, height)),
          current_directory_rect(create_rectangle(0, 0.8 * fsb_to_top_edge_dist, 0.8 * width, 0.1 * height)),
          main_file_view_rect(create_rectangle(0, 0 * height, 0.7 * width, 0.7 * height)),
          file_selection_bar(create_rectangle(0, -0.4 * height, 0.8 * width, 0.1 * height)),
          open_button(create_rectangle(0.4 * width, -0.4 * height, 0.1 * width, 0.1 * height)),
          close_button(create_rectangle(0.4 * width, 0.4 * height, 0.05 * width, 0.05 * height)),
          up_a_dir_button(create_rectangle(-0.4 * width, 0.4 * height, 0.05 * width, 0.05 * height)) {}
};

struct UIRect {
    int id = UniqueIDGenerator::generate();
    IVPSolidColor ivpsc;
    bool mouse_above = false;
    UIRect(IVPSolidColor ivpsc) : ivpsc(ivpsc) {};
};

struct UITextBox {
    int id = UniqueIDGenerator::generate();
    IVPSolidColor background_ivpsc;
    IVPTextured text_drawing_data;
    Rectangle bounding_rect;
    bool mouse_above = false;
    TemporalBinarySignal modified_signal;

    UITextBox(IVPSolidColor background_ivpsc, IVPTextured text_drawing_data, Rectangle bounding_rect)
        : background_ivpsc(background_ivpsc), text_drawing_data(text_drawing_data), bounding_rect(bounding_rect) {};
};

struct UIClickableTextBox {
    int id = UniqueIDGenerator::generate();
    std::function<void()> on_click;
    std::function<void()> on_hover;
    IVPSolidColor ivpsc;
    IVPTextured text_drawing_data;
    glm::vec3 regular_color;
    glm::vec3 hover_color;
    Rectangle rect;
    bool mouse_inside = false;
    TemporalBinarySignal modified_signal;

    UIClickableTextBox(std::function<void()> on_click, std::function<void()> on_hover, IVPSolidColor ivpsc,
                       IVPTextured text_drawing_data, glm::vec3 regular_color, glm::vec3 hover_color, Rectangle rect)
        : on_click(on_click), on_hover(on_hover), ivpsc(ivpsc), text_drawing_data(text_drawing_data),
          regular_color(regular_color), hover_color(hover_color), rect(rect) {}
};

struct UIInputBox {
    int id = UniqueIDGenerator::generate();
    std::function<void(std::string)> on_confirm;
    IVPSolidColor background_ivpsc;
    IVPTextured text_drawing_data;
    std::string placeholder_text;
    std::string contents;
    glm::vec3 regular_color;
    glm::vec3 focused_color;
    Rectangle rect;
    bool focused = false;

    UIInputBox(std::function<void(std::string)> on_confirm, IVPSolidColor background_ivpsc,
               IVPTextured text_drawing_data, std::string placeholder_text, std::string contents,
               glm::vec3 regular_color, glm::vec3 focused_color, Rectangle rect)
        : on_confirm(on_confirm), background_ivpsc(background_ivpsc), text_drawing_data(text_drawing_data),
          placeholder_text(placeholder_text), contents(contents), regular_color(regular_color),
          focused_color(focused_color), rect(rect) {}
};

class UI {
  public:
    UI(const FontAtlas &font_atlas) : font_atlas(font_atlas) {};

    glm::vec2 get_ndc_mouse_pos(unsigned int window_width, unsigned int window_height, double xpos, double ypos);
    void process_mouse_position(const glm::vec2 &mouse_pos_ndc);
    void process_mouse_just_clicked(const glm::vec2 &mouse_pos_ndc);
    void process_key_press(const std::string &character_pressed);
    void process_confirm_action();
    void process_delete_action();

    void add_colored_rectangle(Rectangle ndc_rectangle, const glm::vec3 &normalized_rgb);

    void add_colored_rectangle(float x_pos_ndc, float y_pos_ndc, float width, float height,
                               const glm::vec3 &normalized_rgb);

    /*void add_clickable_colored_rectangle(std::function<void()> on_click, float x_pos_ndc, float y_pos_ndc, float
     * width,*/
    /*                                     float height, const glm::vec3 &regular_color, const glm::vec3
     * &hover_color);*/

    int add_textbox(const std::string &text, Rectangle ndc_text_rectangle, const glm::vec3 &normalized_rgb);

    int add_textbox(const std::string &text, float center_x_pos_ndc, float center_y_pos_ndc, float width, float height,
                    const glm::vec3 &normalized_rgb);

    void modify_text_of_a_textbox(int doid, std::string new_text);

    UITextBox *get_textbox(int doid);

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
    int add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                              float x_pos_ndc, float y_pos_ndc, float width, float height,
                              const glm::vec3 &regular_color, const glm::vec3 &hover_color);

    // in the future these probably don't have to be nodiscard so long as a I have a method for geting ids back out.
    int add_clickable_textbox(std::function<void()> &on_click, std::function<void()> &on_hover, const std::string &text,
                              Rectangle &rect, const glm::vec3 &regular_color, const glm::vec3 &hover_color);

    bool remove_clickable_textbox(int do_id);
    bool remove_textbox(int do_id);
    UIClickableTextBox *get_clickable_textbox(int do_id);

    // same version without function references, so we can create generic throwaway functions if we need to
    void add_clickable_textbox_copy_fun(std::function<void()> on_click, std::function<void()> on_hover,
                                        const std::string &text, Rectangle &rect, const glm::vec3 &regular_color,
                                        const glm::vec3 &hover_color);

    void add_clickable_textbox_copy_fun(std::function<void()> on_click, std::function<void()> on_hover,
                                        const std::string &text, float x_pos_ndc, float y_pos_ndc, float width,
                                        float height, const glm::vec3 &regular_color, const glm::vec3 &hover_color);

    void add_input_box(std::function<void(std::string)> &on_confirm, const std::string &placeholder_text,
                       float x_pos_ndc, float y_pos_ndc, float width, float height, const glm::vec3 &regular_color,
                       const glm::vec3 &focused_color);

    /*const std::vector<UIRect> &get_rectangles() const;*/
    /*const std::vector<UIClickableTextBox> &get_clickable_text_boxes() const;*/
    /*const std::vector<IVPTextured> &get_text_boxes() const;*/

    const std::vector<UIClickableTextBox> &get_clickable_text_boxes() const;
    const std::vector<UIInputBox> &get_input_boxes() const;
    const std::vector<UITextBox> &get_text_boxes() const;
    const std::vector<UIRect> &get_colored_boxes() const;

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
