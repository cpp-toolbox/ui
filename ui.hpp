#ifndef UI_HPP
#define UI_HPP

#include <functional>
#include <glm/fwd.hpp>
#include <optional>
#include "sbpt_generated_includes.hpp"

struct FileBrowser {
    float fsb_width;
    float fsb_height;
    float fsb_to_top_edge_dist;

    vertex_geometry::Rectangle background_rect;
    vertex_geometry::Rectangle current_directory_rect;
    vertex_geometry::Rectangle main_file_view_rect;
    vertex_geometry::Rectangle file_selection_bar;
    vertex_geometry::Rectangle open_button;
    vertex_geometry::Rectangle close_button;
    vertex_geometry::Rectangle up_a_dir_button;

    // Constructor to initialize the struct with the necessary parameters
    FileBrowser(float width, float height)
        : fsb_width(width), fsb_height(height), fsb_to_top_edge_dist(height / 2.0),
          background_rect(vertex_geometry::create_rectangle(0, 0, width, height)),
          current_directory_rect(
              vertex_geometry::create_rectangle(0, 0.8 * fsb_to_top_edge_dist, 0.8 * width, 0.1 * height)),
          main_file_view_rect(vertex_geometry::create_rectangle(0, 0 * height, 0.7 * width, 0.7 * height)),
          file_selection_bar(vertex_geometry::create_rectangle(0, -0.4 * height, 0.8 * width, 0.1 * height)),
          open_button(vertex_geometry::create_rectangle(0.4 * width, -0.4 * height, 0.1 * width, 0.1 * height)),
          close_button(vertex_geometry::create_rectangle(0.4 * width, 0.4 * height, 0.05 * width, 0.05 * height)),
          up_a_dir_button(vertex_geometry::create_rectangle(-0.4 * width, 0.4 * height, 0.05 * width, 0.05 * height)) {}
};

struct UIRect {
    int parent_ui_id;
    draw_info::IVPColor ivpsc;
    bool mouse_above = false;
    TemporalBinarySignal modified_signal;

    bool hidden = false;

    // TODO: why are we using the global here...
    UIRect(draw_info::IVPColor ivpsc, int id = GlobalUIDGenerator::get_id()) : parent_ui_id(id), ivpsc(ivpsc) {}
};

struct UITextBox {
    int parent_ui_id;
    draw_info::IVPColor background_ivpsc;
    draw_info::IVPColor text_drawing_ivpsc;
    vertex_geometry::Rectangle bounding_rect;
    bool mouse_above = false;
    TemporalBinarySignal modified_signal;

    bool hidden = false;

    UITextBox(draw_info::IVPColor background_ivpsc, draw_info::IVPColor text_drawing_data,
              vertex_geometry::Rectangle bounding_rect, int id = GlobalUIDGenerator::get_id())
        : background_ivpsc(background_ivpsc), text_drawing_ivpsc(text_drawing_data), bounding_rect(bounding_rect),
          parent_ui_id(id) {};
};

struct UIClickableTextBox {
    int id;
    std::function<void()> on_click;
    std::function<void()> on_hover;
    draw_info::IVPColor ivpsc;
    draw_info::IVPColor text_drawing_ivpsc;
    glm::vec3 regular_color;
    glm::vec3 hover_color;
    vertex_geometry::Rectangle rect;
    bool mouse_inside = false;
    TemporalBinarySignal modified_signal;

    UIClickableTextBox(std::function<void()> on_click, std::function<void()> on_hover, draw_info::IVPColor ivpsc,
                       draw_info::IVPColor text_drawing_data, glm::vec3 regular_color, glm::vec3 hover_color,
                       vertex_geometry::Rectangle rect, int id = GlobalUIDGenerator::get_id())
        : on_click(on_click), on_hover(on_hover), ivpsc(ivpsc), text_drawing_ivpsc(text_drawing_data),
          regular_color(regular_color), hover_color(hover_color), rect(rect), id(id) {}
};

struct UIDropdownOption {

    UIDropdownOption(std::string option, glm::vec3 color, glm::vec3 hover_color, draw_info::IVPColor background_ivpsc,
                     draw_info::IVPColor text_ivpsc, vertex_geometry::Rectangle rect,
                     std::function<void(const std::string)> on_click, std::function<void(const std::string)> on_hover)
        : option(std::move(option)), color(color), hover_color(hover_color),
          background_ivpsc(std::move(background_ivpsc)), text_ivpsc(std::move(text_ivpsc)), rect(rect),
          on_click(std::move(on_click)), on_hover(std::move(on_hover)) {}

    std::string option;

    glm::vec3 color;
    glm::vec3 hover_color;

    draw_info::IVPColor background_ivpsc;
    draw_info::IVPColor text_ivpsc;

    TemporalBinarySignal modified_signal;

    vertex_geometry::Rectangle rect;

    bool mouse_inside = false;
    std::function<void(const std::string)> on_click;
    std::function<void(const std::string)> on_hover;
};

struct UIDropdown {
    int id;

    std::string selected_option;

    glm::vec3 regular_color;
    glm::vec3 hover_color;

    std::function<void()> on_click;
    std::function<void()> on_hover;

    draw_info::IVPColor dropdown_background;
    draw_info::IVPColor dropdown_text_ivpsc;
    vertex_geometry::Rectangle dropdown_rect;

    // a vector because we have one for each dropdown elt

    std::vector<UIDropdownOption> ui_dropdown_options;
    bool mouse_inside = false;
    bool dropdown_open = false;
    TemporalBinarySignal modified_signal;

    UIDropdown(std::function<void()> on_click, std::function<void()> on_hover, draw_info::IVPColor dropdown_background,
               draw_info::IVPColor dropdown_text_data, glm::vec3 regular_color, glm::vec3 hover_color,
               vertex_geometry::Rectangle dropdown_rect, std::vector<UIDropdownOption> ui_dropdown_options,
               int id = GlobalUIDGenerator::get_id())
        : on_click(on_click), on_hover(on_hover), dropdown_background(dropdown_background),
          dropdown_text_ivpsc(dropdown_text_data), regular_color(regular_color), hover_color(hover_color),
          dropdown_rect(dropdown_rect), ui_dropdown_options(ui_dropdown_options), id(id) {
        // NOTE:  we're running under the assumption that every dropdown will have at least one option
        selected_option = ui_dropdown_options.at(0).option;
    }
};

struct UIInputBox {
    int id;
    std::function<void(std::string)> on_confirm;
    draw_info::IVPColor background_ivpsc;
    draw_info::IVPColor text_drawing_ivpsc;
    std::string placeholder_text;
    std::string contents;
    glm::vec3 regular_color;
    glm::vec3 focused_color;
    vertex_geometry::Rectangle rect;
    bool focused = false;
    TemporalBinarySignal modified_signal;
    /// @note sometimes you have a key which initializes the input box, and you want to ignore that stroke so that you
    /// don't get an initial character in the box.
    std::optional<std::string> initial_ignore_character;
    bool already_ignored_initial_character_during_active_focus = false;

    UIInputBox(std::function<void(std::string)> on_confirm, draw_info::IVPColor background_ivpsc,
               draw_info::IVPColor text_drawing_data, std::string placeholder_text, std::string contents,
               glm::vec3 regular_color, glm::vec3 focused_color, vertex_geometry::Rectangle rect,
               int id = GlobalUIDGenerator::get_id(),
               std::optional<std::string> initial_ignore_character = std::nullopt)
        : on_confirm(on_confirm), background_ivpsc(background_ivpsc), text_drawing_ivpsc(text_drawing_data),
          placeholder_text(placeholder_text), contents(contents), regular_color(regular_color),
          focused_color(focused_color), rect(rect), id(id), initial_ignore_character(initial_ignore_character) {}
};

// we work in ndc space, and so z layer refers to what z layer we're on, by default we have 20 z layers of the form
// -1, -0.9, -0.8, ..., -0.1, 0, 0.1, 0.2, 0.3, ... 0.9, 1
// each UI owns the space from its z layer to the next z layer, so for example if you create a UI with z layer 0, then
// it owns the space from 0 to 0.1 and thus all the numbers like 0.01, 0.02, 0.03, 0.04, ... 0.09 are all bandwiths you
// can do stuff on safely and not hit the next z layer.
//
// TODO: in the future I think I can remove the abs_pos_object id generator because the batcher can register ids
// automatically now
class UI {
  public:
    UI(float z_layer, UniqueIDGenerator &abs_pos_object_id_generator)
        : z_layer(z_layer), background_layer(z_layer - 0.01), text_layer(z_layer - 0.02),
          dropdown_background_layer(z_layer - 0.03), dropdown_text_layer(z_layer - 0.04),
          abs_pos_object_id_generator(abs_pos_object_id_generator) {};

    float z_layer, background_layer, text_layer, dropdown_background_layer, dropdown_text_layer;

    UniqueIDGenerator ui_id_generator;

    // TODO: I don't like that these are here, but I don't really have a choice right now correct?
    UniqueIDGenerator &abs_pos_object_id_generator;

    void process_mouse_position(const glm::vec2 &mouse_pos_ndc);

    void process_mouse_just_clicked(const glm::vec2 &mouse_pos_ndc);
    bool process_mouse_just_clicked_on_clickable_textboxes(const glm::vec2 &mouse_pos_ndc);
    bool process_mouse_just_clicked_on_input_boxes(const glm::vec2 &mouse_pos_ndc);
    bool process_mouse_just_clicked_on_dropdown_options(const glm::vec2 &mouse_pos_ndc);
    bool process_mouse_just_clicked_on_dropdowns(const glm::vec2 &mouse_pos_ndc);

    void unfocus_input_box(UIInputBox &ib);
    void unfocus_input_box(int input_box_eid);

    void focus_input_box(UIInputBox &ib);
    void focus_input_box(int input_box_eid);

    void update_dropdown_option(UIDropdown &dropdown, const std::string &option_name);

    void process_key_press(const std::string &character_pressed);
    void process_confirm_action();
    void process_delete_action();

    /**
     * @brief adds a colored retangle the ui.
     *
     */
    void add_colored_rectangle(vertex_geometry::Rectangle ndc_rectangle, const glm::vec3 &normalized_rgb);

    // WARN: deprecated
    void add_colored_rectangle(float x_pos_ndc, float y_pos_ndc, float width, float height,
                               const glm::vec3 &normalized_rgb);

    /*void add_clickable_colored_rectangle(std::function<void()> on_click, float x_pos_ndc, float y_pos_ndc, float
     * width,*/
    /*                                     float height, const glm::vec3 &regular_color, const glm::vec3
     * &hover_color);*/

    int add_textbox(const std::string &text, vertex_geometry::Rectangle ndc_text_rectangle,
                    const glm::vec3 &normalized_rgb);
    int add_textbox(const std::string &text, float center_x_pos_ndc, float center_y_pos_ndc, float width, float height,
                    const glm::vec3 &normalized_rgb);

    void hide_textbox(int doid);
    void unhide_textbox(int doid);
    void modify_text_of_a_textbox(int doid, std::string new_text);
    void modify_colored_rectangle(int doid, vertex_geometry::Rectangle ndc_rectangle);

    // TODO: just return references, I did pointers because I didn't know I could return references I think
    UITextBox *get_textbox(int doid);
    UIInputBox *get_inputbox(int doid);
    UIRect *get_colored_rectangle(int doid);

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

    /*
     * we pass by copy here because if the functions go out of scope and we captured by reference, then the references
     * would no longer exist, that actually happens when you were to create a ui in a function and define the
     * onclick/onhover functions in there.
     */
    int add_clickable_textbox(std::function<void()> on_click, std::function<void()> on_hover, const std::string &text,
                              float x_pos_ndc, float y_pos_ndc, float width, float height,
                              const glm::vec3 &regular_color, const glm::vec3 &hover_color);

    // in the future these probably don't have to be nodiscard so long as a I have a method for geting ids back out.
    int add_clickable_textbox(std::function<void()> on_click, std::function<void()> on_hover, const std::string &text,
                              vertex_geometry::Rectangle &rect, const glm::vec3 &regular_color,
                              const glm::vec3 &hover_color);

    /**
     * @brief Adds a dropdown UI element with a main button and multiple selectable options.
     *
     * This function creates a dropdown menu consisting of a main button (displaying one of the given options)
     * and a list of selectable dropdown options. Each element in the dropdown (main button and options) is
     * represented as rectangles with text overlays, and is assigned unique internal IDs for rendering and event
     * handling.
     *
     * @param on_click Callback triggered when the main dropdown button is clicked.
     * @param on_hover Callback triggered when the main dropdown button is hovered over.
     * @param dropdown_option_idx Index of the option in @p options to display as the initially selected option.
     * @param rect The rectangle defining the size and position of the dropdown button.
     * @param regular_color The background color of the main dropdown button in its normal state.
     * @param hover_color The background color of the main dropdown button when hovered over.
     * @param options A list of strings representing the dropdown options to display.
     * @param option_on_click Callback triggered when a dropdown option is clicked. Receives the option string as input.
     * @param option_on_hover Callback triggered when a dropdown option is hovered over. Receives the option string as
     * input.
     * @param option_color The background color of dropdown options in their normal state.
     *                     If (0,0,0), a default darker variant of @p regular_color is used.
     * @param option_hover_color The background color of dropdown options when hovered.
     *                           If (0,0,0), a default darker variant of @p hover_color is used.
     *
     * @return int A unique ID representing the created dropdown element. This ID can be used to reference
     *             or remove the dropdown from the UI system later.
     *
     * @note
     * - Each dropdown option is positioned below the main button by sliding the rectangle downward.
     * - Colors default to dimmed versions of the main button colors if option-specific colors are not provided.
     * - All internal geometry is generated and registered with unique IDs for rendering and event management.
     *
     * @bug @p options must not be empty or else it crashes, also @p dropdown_option_idx must be a valid index of @p
     * options
     *
     * @see UIDropdown, UIDropdownOption
     */
    int add_dropdown(std::function<void()> on_click, std::function<void()> on_hover, int dropdown_option_idx,
                     const vertex_geometry::Rectangle &rect, const glm::vec3 &regular_color,
                     const glm::vec3 &hover_color, const std::vector<std::string> &options,
                     std::function<void(std::string)> option_on_click, std::function<void(std::string)> option_on_hover,
                     const glm::vec3 &option_color = glm::vec3(0), const glm::vec3 &option_hover_color = glm::vec3(0));

    // TODO: for now I'm not going to trust these remove methods I think.
    bool remove_clickable_textbox(int do_id);
    bool remove_textbox(int do_id);
    UIClickableTextBox *get_clickable_textbox(int do_id);

    int add_input_box(std::function<void(std::string)> on_confirm, const std::string &placeholder_text,
                      const vertex_geometry::Rectangle &ndc_rect, const glm::vec3 &regular_color,
                      const glm::vec3 &focused_color,
                      std::optional<std::string> initial_ignore_character = std::nullopt);

    int add_input_box(std::function<void(std::string)> on_confirm, const std::string &placeholder_text, float x_pos_ndc,
                      float y_pos_ndc, float width, float height, const glm::vec3 &regular_color,
                      const glm::vec3 &focused_color,
                      std::optional<std::string> initial_ignore_character = std::nullopt);

    /*const std::vector<UIRect> &get_rectangles() const;*/
    /*const std::vector<UIClickableTextBox> &get_clickable_text_boxes() const;*/
    /*const std::vector<IVPTextured> &get_text_boxes() const;*/

    const std::vector<UIClickableTextBox> &get_clickable_text_boxes() const;
    const std::vector<UIDropdown> &get_dropdowns() const;
    const std::vector<UIInputBox> &get_input_boxes() const;
    const std::vector<UITextBox> &get_text_boxes() const;
    const std::vector<UIRect> &get_colored_boxes() const;

  private:
    void disable_focus_on_all_input_boxes();

    std::vector<draw_info::IVPTextured> drawable_text_information;

    // NOTE: these are used for checking mouse clicks
    std::vector<UIRect> rectangles;
    std::vector<UIDropdown> dropdowns;
    std::vector<UIClickableTextBox> clickable_text_boxes;
    std::vector<UITextBox> text_boxes;
    std::vector<UIInputBox> input_boxes;
};

/**
 * @class IUIRenderSuite
 * @brief Interface for UI rendering operations.
 */
class IUIRenderSuite {
  public:
    virtual ~IUIRenderSuite() = default;

    /**
     * @brief Render a colored box.
     * @param cb The UIRect containing rendering data.
     */
    virtual void render_colored_box(const UIRect &cb) = 0;

    /**
     * @brief Render a text box.
     * @param tb The UITextBox containing text and background data.
     */
    virtual void render_text_box(const UITextBox &tb) = 0;

    /**
     * @brief Render a clickable text box.
     * @param cr The UIClickableTextBox containing text and background data.
     */
    virtual void render_clickable_text_box(const UIClickableTextBox &cr) = 0;

    /**
     * @brief Render an input box.
     * @param ib The UIInputBox containing text and background data.
     */
    virtual void render_input_box(const UIInputBox &ib) = 0;

    /**
     * @brief Render a dropdown menu.
     * @param dd The UIDropdown containing text and background data.
     */
    virtual void render_dropdown(const UIDropdown &dd) = 0;

    virtual void render_dropdown_option(const UIDropdownOption &udo) = 0;
};

/**
 * @brief the function that actually renders the ui
 *
 *
 */
void process_and_queue_render_ui(glm::vec2 ndc_mouse_pos, UI &curr_ui, IUIRenderSuite &ui_render_suite,
                                 const std::vector<std::string> &key_strings_just_pressed,
                                 bool delete_action_just_pressed, bool confirm_action_just_pressed,
                                 bool mouse_just_clicked);

#endif // UI_HPP
