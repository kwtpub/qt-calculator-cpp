#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cctype>

class Parser {
    std::string input;
    size_t pos;

    void skipWhitespace() {
        while (pos < input.length() && isspace(input[pos])) pos++;
    }

    void checkPreConditions(const std::string& str) {
        if (str.empty()) throw std::runtime_error("Пустое выражение");
        
        std::string ops = "+-*/";
        for (size_t i = 0; i < str.length() - 1; i++) {
            if (ops.find(str[i]) != std::string::npos) {
                size_t next = i + 1;
                while (next < str.length() && isspace(str[next])) next++;
                if (next < str.length()) {
                    if (str[i] == '+' && str[next] == '+') throw std::runtime_error("Недопустимая последовательность '++'");
                    if (str[i] == '-' && str[next] == '-') throw std::runtime_error("Недопустимая последовательность '--'");
                    if (ops.find(str[next]) != std::string::npos && str[next] != '-') {
                        throw std::runtime_error("Недопустимая последовательность операторов");
                    }
                }
            }
        }
    }

    double parseFactor() {
        skipWhitespace();
        if (pos >= input.length()) throw std::runtime_error("Неожиданный конец выражения");

        if (input[pos] == '(') {
            pos++;
            double val = parseExpression();
            skipWhitespace();
            if (pos >= input.length() || input[pos] != ')') throw std::runtime_error("Пропущена закрывающая скобка");
            pos++;
            return val;
        } else if (input[pos] == '+' || input[pos] == '-') {
            char op = input[pos++];
            double val = parseFactor();
            return (op == '-') ? -val : val;
        } else if (isalpha(input[pos])) {
            std::string func = "";
            while (pos < input.length() && isalpha(input[pos])) {
                func += input[pos++];
            }
            skipWhitespace();
            if (pos >= input.length() || input[pos] != '(') throw std::runtime_error("Ожидается '(' после " + func);
            pos++;
            double val = parseExpression();
            skipWhitespace();
            
            double val2 = 0;
            bool hasTwoArgs = false;
            if (func == "log" && pos < input.length() && input[pos] == ',') {
                pos++;
                val2 = parseExpression();
                skipWhitespace();
                hasTwoArgs = true;
            }

            if (pos >= input.length() || input[pos] != ')') throw std::runtime_error("Пропущена закрывающая скобка для " + func);
            pos++;
            
            if (func == "sin") return std::sin(val);
            else if (func == "cos") return std::cos(val);
            else if (func == "tan") return std::tan(val);
            else if (func == "log") {
                if (hasTwoArgs) {
                    if (val <= 0 || val == 1) throw std::runtime_error("Недопустимое основание логарифма");
                    if (val2 <= 0) throw std::runtime_error("Недопустимый аргумент логарифма");
                    return std::log(val2) / std::log(val);
                } else {
                    if (val <= 0) throw std::runtime_error("Недопустимый аргумент логарифма");
                    return std::log10(val);
                }
            }
            else throw std::runtime_error("Неизвестная функция: " + func);
        } else if (isdigit(input[pos]) || input[pos] == '.') {
            size_t startPos = pos;
            bool hasDot = false;
            while (pos < input.length() && (isdigit(input[pos]) || input[pos] == '.')) {
                if (input[pos] == '.') {
                    if (hasDot) throw std::runtime_error("Несколько десятичных точек");
                    hasDot = true;
                }
                pos++;
            }
            std::string numStr = input.substr(startPos, pos - startPos);
            if (numStr == ".") throw std::runtime_error("Недопустимое число '.'");
            return std::stod(numStr);
        }

        throw std::runtime_error("Неожиданный символ");
    }

    double parseTerm() {
        double val = parseFactor();
        skipWhitespace();
        while (pos < input.length() && (input[pos] == '*' || input[pos] == '/')) {
            char op = input[pos++];
            double nextVal = parseFactor();
            if (op == '*') {
                val *= nextVal;
            } else {
                if (nextVal == 0) throw std::runtime_error("Деление на ноль");
                val /= nextVal;
            }
            skipWhitespace();
        }
        return val;
    }

    double parseExpression() {
        double val = parseTerm();
        skipWhitespace();
        while (pos < input.length() && (input[pos] == '+' || input[pos] == '-')) {
            char op = input[pos++];
            double nextVal = parseTerm();
            if (op == '+') val += nextVal;
            else val -= nextVal;
            skipWhitespace();
        }
        return val;
    }

public:
    double evaluate(const std::string& expr) {
        checkPreConditions(expr);
        input = expr;
        pos = 0;
        double result = parseExpression();
        skipWhitespace();
        if (pos < input.length()) {
            throw std::runtime_error("Пропущены операторы или недопустимые символы");
        }
        return result;
    }
};

GtkWidget *entry_input;
GtkWidget *label_result;

static void on_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *button_label = gtk_button_get_label(GTK_BUTTON(widget));
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry_input));
    
    std::string new_text = current_text;
    std::string lbl = button_label;
    if (lbl == "sin" || lbl == "cos" || lbl == "tan" || lbl == "log") {
        new_text += lbl + "(";
    } else {
        new_text += lbl;
    }
    
    gtk_entry_set_text(GTK_ENTRY(entry_input), new_text.c_str());
    gtk_editable_set_position(GTK_EDITABLE(entry_input), -1);
}

static void on_clear_clicked(GtkWidget *widget, gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(entry_input), "");
    gtk_label_set_markup(GTK_LABEL(label_result), "<span size='large' weight='bold'>Result: </span>");
}

static void on_backspace_clicked(GtkWidget *widget, gpointer data) {
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry_input));
    std::string text = current_text;
    if (!text.empty()) {
        text.pop_back();
        gtk_entry_set_text(GTK_ENTRY(entry_input), text.c_str());
        gtk_editable_set_position(GTK_EDITABLE(entry_input), -1);
    }
}

static void on_equal_clicked(GtkWidget *widget, gpointer data) {
    const gchar *expr = gtk_entry_get_text(GTK_ENTRY(entry_input));
    Parser parser;
    try {
        double result = parser.evaluate(expr);
        std::ostringstream ss;
        ss << "Result: " << result;
        
        gchar *markup = g_markup_printf_escaped("<span size='large' weight='bold'>%s</span>", ss.str().c_str());
        gtk_label_set_markup(GTK_LABEL(label_result), markup);
        g_free(markup);
    } catch (const std::exception& e) {
        std::string err = std::string("Ошибка: ") + e.what();
        gchar *markup = g_markup_printf_escaped("<span size='large' weight='bold' foreground='red'>%s</span>", err.c_str());
        gtk_label_set_markup(GTK_LABEL(label_result), markup);
        g_free(markup);
    }
}

// Allow pressing Enter key to evaluate
static void on_entry_activate(GtkEntry *entry, gpointer data) {
    on_equal_clicked(NULL, NULL);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "C++ Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 450);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    entry_input = gtk_entry_new();
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "entry { font-size: 1.5em; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(entry_input),
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    
    g_signal_connect(entry_input, "activate", G_CALLBACK(on_entry_activate), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), entry_input, FALSE, FALSE, 0);
    
    label_result = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label_result), "<span size='large' weight='bold'>Result: </span>");
    gtk_widget_set_halign(label_result, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), label_result, FALSE, FALSE, 5);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    const char* buttons[5][5] = {
        {"sin", "cos", "tan", "log", ","},
        {"7", "8", "9", "/", "C"},
        {"4", "5", "6", "*", "<-"},
        {"1", "2", "3", "-", "("},
        {"0", ".", "=", "+", ")"}
    };

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            GtkWidget *btn = gtk_button_new_with_label(buttons[i][j]);
            gtk_widget_set_hexpand(btn, TRUE);
            gtk_widget_set_vexpand(btn, TRUE);
            
            GtkStyleContext *context = gtk_widget_get_style_context(btn);
            if (std::string(buttons[i][j]) == "=") {
                gtk_style_context_add_class(context, "suggested-action");
            } else if (std::string(buttons[i][j]) == "C" || std::string(buttons[i][j]) == "<-") {
                gtk_style_context_add_class(context, "destructive-action");
            }

            if (std::string(buttons[i][j]) == "=") {
                g_signal_connect(btn, "clicked", G_CALLBACK(on_equal_clicked), NULL);
            } else if (std::string(buttons[i][j]) == "C") {
                g_signal_connect(btn, "clicked", G_CALLBACK(on_clear_clicked), NULL);
            } else if (std::string(buttons[i][j]) == "<-") {
                g_signal_connect(btn, "clicked", G_CALLBACK(on_backspace_clicked), NULL);
            } else {
                g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
            }
            
            gtk_grid_attach(GTK_GRID(grid), btn, j, i, 1, 1);
        }
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}