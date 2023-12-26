#include <windows.h>
#include <cstdio>
#include <vector>
#include <fstream>
#include <map>

const char* KEY_CONFIG_FILE = "settings.txt";

const int SIMULATED_KEY_Q = 0x44;
const int SIMULATED_KEY_D = 0x51;

static std::string get_key_name(int code) {
    std::map<int, std::string> dict = {
        {0x04, "Bouton Milieu Souris"},
        {0x05, "Bouton Gauche Arrière souris"},
    };
    auto item = dict.find(code);
    if (item != dict.end()) {
        return item->second;
    }
    else {
        return "Inconnu";
    }
}

static int read_file() {
    std::ifstream file(KEY_CONFIG_FILE);
    int saved_key;
    if (file >> std::hex >> saved_key) {
        return saved_key;
    }
    return 0x04;
}

static void write_file(int key) {
    std::ofstream file(KEY_CONFIG_FILE);
    if (file) {
        file << std::hex << key;
        file.close();
    }
    else {
        fprintf(stderr, "Impossible d'écrire dans le fichier\n");
    }
}

static bool is_valid_key(int key, int current_key) {
    static const std::vector<int> allowed_keys = { 0x04, 0x05 };
    for (int x : allowed_keys) {
        if (x == key && x != current_key) {
            return true;
        }
    }
    return false;
}

void key_script() {
    int default_key = read_file();
    bool waiting_for_key = false;

    for (;;) {
        if (GetAsyncKeyState(default_key) & 0x8000) {
            keybd_event(SIMULATED_KEY_Q, 0, 0, 0);
            Sleep(109);
            keybd_event(SIMULATED_KEY_Q, 0, KEYEVENTF_KEYUP, 0);
            Sleep(10);
            keybd_event(SIMULATED_KEY_D, 0, 0, 0);
            Sleep(125);
            keybd_event(SIMULATED_KEY_D, 0, KEYEVENTF_KEYUP, 0);
        }

        if (!waiting_for_key && (GetAsyncKeyState(VK_END) & 0x0001)) {
            for (int key = 0x04; key <= 0xFF; key++) {
                GetAsyncKeyState(key);
            }

            printf("Entre un nouveau raccourci\n");

            waiting_for_key = true;

            bool key_pressed = false;
            while (!key_pressed) {
                for (int key = 0x04; key <= 0xFE; key++) {
                    if (GetAsyncKeyState(key) & 0x0001) {
                        if (is_valid_key(key, default_key)) {
                            printf("Nouveau raccourci : %s\n", get_key_name(key).c_str());
                            default_key = key;
                            key_pressed = true;
                            write_file(key);
                        }
                        else {
                            printf("%s\n", (key == default_key ? "Ce raccourci est déjà utilisé" : "Raccourci invalide (BGS ou BMS uniquement)"));
                        }
                        break;
                    }
                }
                Sleep(200);
            }
            waiting_for_key = false;
        }
    }
}
