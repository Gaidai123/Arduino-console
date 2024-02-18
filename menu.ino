#include <EEPROM.h>
#include <GyverOLED.h>
#include <GyverJoy.h>

#define joy_x_pin 2
#define joy_y_pin 1
#define joy_button 3

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled; //sda - a4, scl - a5
GyverJoy jx(2);  
GyverJoy jy(1);

int joy_x, joy_y;
#define DEBUG 1 //1 - joy, 2 - game_debug

enum {
    PINGPONG_RECORD,
    SNAKE_RECORD,
} eeprom;
enum {
    MENU,
    PINGPONG,
    SNAKE,
    TICTACTOE,
} game;
enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    NONE,
} rotate_enum;



void setup() {
#if (DEBUG == 1 || DEBUG == 2)
    Serial.begin(9600);
#endif

    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(3, INPUT_PULLUP); //ÐºÐ½Ð¾Ð¿ÐºÐ°

    oled.init();
    oled.setScale(3);

    randomSeed(analogRead(A0));

    jx.calibrate();         // ÐºÐ°Ð»Ð¸Ð±ÑÐ¾Ð²ÐºÐ° Ð½ÑÐ»Ñ Ð¿ÑÐ¸ Ð·Ð°Ð¿ÑÑÐºÐµ
    jx.deadzone(30);        // Ð¼ÑÑÑÐ²Ð°Ñ Ð·Ð¾Ð½Ð°
    jx.exponent(GJ_CUBIC);  // ÑÐºÑÐ¿Ð¾Ð½ÐµÐ½ÑÐ° Ð´Ð»Ñ Ð¿Ð»Ð°Ð²Ð½Ð¾ÑÑÐ¸
    jy.calibrate();         // ÐºÐ°Ð»Ð¸Ð±ÑÐ¾Ð²ÐºÐ° Ð½ÑÐ»Ñ Ð¿ÑÐ¸ Ð·Ð°Ð¿ÑÑÐºÐµ
    jy.deadzone(30);        // Ð¼ÑÑÑÐ²Ð°Ñ Ð·Ð¾Ð½Ð°
    jy.exponent(GJ_CUBIC);  // ÑÐºÑÐ¿Ð¾Ð½ÐµÐ½ÑÐ° Ð´Ð»Ñ Ð¿Ð»Ð°Ð²Ð½Ð¾ÑÑÐ¸
}

void loop() {
    static byte game = MENU;

    //////////// ÐÐ±ÑÐ°Ð±Ð¾ÑÐºÐ° Ð´Ð°ÑÑÐ¸ÐºÐ¾Ð² ////////////
    if (jx.tick()) {
        joy_y = -jx.value();
    }
    if (jy.tick()) {
        joy_x = jy.value();
    }


    //////////// Ð Ð°Ð±Ð¾ÑÐ° Ñ ÑÐºÑÐ°Ð½Ð¾Ð¼ ////////////
    oled.clear();

    if (game == MENU) {
        const byte max_game = 3;
        const PROGMEM char* game_s[max_game] = 
        { "ÐÐ¸Ð½Ð³-ÐÐ¾Ð½Ð³", "ÐÐ¼ÐµÐ¹ÐºÐ°", "ÐÑÐµÑÑÐ¸ÐºÐ¸"};
        static int8_t i = 0;
        static int8_t choice = 0;
        static unsigned long last_time;
        static bool is;
        static bool flag = 0;

        oled.setScale(2);
        oled.setCursor(2, 0);
        oled.print(game_s[i]);
        oled.setCursor(2, 3);
        oled.print(game_s[i + 1]);
        oled.setCursor(2, 6);
        oled.print(game_s[i + 2]);

        menurect_draw(choice);

        if (joy_y != -1) {
            is = 1;
        } else {
            is = 0;
        }

        if (is && !flag && ((millis() - last_time) > 100)) {
            flag = true;
            last_time = millis();
            if (joy_y > -1) {
                choice--;
            } else if (joy_y < -1) {
                choice++;
            }
        }
        if (is && flag && millis() - last_time > 800) {
            last_time = millis();
            if (joy_y > -1) {
                choice--;
            } else if (joy_y < -1) {
                choice++;
            }
        }
        if (!is && flag && millis() - last_time > 500) {
            flag = false;
            last_time = millis();
        }

        if (choice > 2) {
            choice = 2;
            i++;
        } else if (choice < 0) {
            choice = 0;
            i--;
        }

        if (!digitalRead(joy_button) == 1) {
            delay(500);
            switch (choice) {
                case 0:
                    game = i + 1;
                    break;
                case 1:
                    game = i + 2;
                    break;
                case 2:
                    game = i + 3;
                    break;
            }
        }
    i = constrain(i, 0, max_game-3);

#if (DEBUG == 1)
        Serial.print(choice);
        Serial.print(" ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(game_s[5]);
#endif
    }

    switch(game) {
        case PINGPONG:
            pingpong_play();
            game = MENU;
            break;
        case SNAKE:
            snake_play();
            game = MENU;
            break;
        case TICTACTOE:
            tic_tac_toe_play();
            game = MENU;
            break;
    }

    oled.update();

#if (DEBUG == 2)
    Serial.print("x = ");
    Serial.print(joy_x);
    Serial.print(", y = ");
    Serial.println(joy_y);
#endif
}

void menurect_draw(int y) {
    oled.rect(0, ((y == 0) ? 0 : 18 * y) + ((y == 0) ? 0 : 4 * y), 127, ((y == 0) ? 18 : 18 * (y + 1)) + ((y == 0) ? 0 : 4 * y), OLED_STROKE);
}