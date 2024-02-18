void pingpong_play(byte* game) {
    byte action = 1;
    byte score = 0;

    struct AABB {
        int x, y;
        int8_t Vx, Vy;
    };

    struct AABB dot;
    struct AABB player;
    struct AABB bot;

    player.x = 12;
    player.y = 26;
    bot.x = 116;
    bot.y = 26;
    bot.Vy = 2;
    dot.Vx = 0;
    dot.Vy = 0;

    while (*game == PINGPONG) {
        oled.clear();

        if (jx.tick()) {
            joy_x = jx.value();
        }
        if (jy.tick()) {
            joy_y = jy.value();
        }

        if (action == 1) {
            bool is_start;
            oled.setScale(3);
            oled.setCursor(12, 2);
            oled.print("Ð¡ÑÐ°ÑÑ?");
            oled.rect(32, 48, 56, 60, OLED_STROKE);
            oled.rect(72, 48, 96, 60, OLED_STROKE);

            if (joy_x < 1) {
                is_start = 1;
            } else if (joy_x > 1) {
                is_start = 0;
            }

            if (is_start == 1) {
                oled.rect(32, 48, 56, 60, OLED_FILL);
                if (!digitalRead(3) == 1) {
                    dot.x = 64;
                    dot.y = 32;
                    player.y = 26;
                    bot.y = 26;
                    randspeed(&dot.Vx, &dot.Vy);
                    action = 2;
                }
            } else if (is_start == 0) {
                oled.rect(72, 48, 96, 60, OLED_FILL);
                if (!digitalRead(3) == 1) {
                    action = 3;
                }
            }
        }

        if (action == 3) {
            oled.clear();
            oled.setScale(3);
            oled.setCursor(48, 3);
            oled.print(":(");
            oled.update();
            delay(2200);
            *game = MENU;
        }

        if (action == 2) {
            dot_move(&dot.x, &dot.y, &dot.Vx, &dot.Vy);
            dot_draw(dot.x, dot.y);
            
            player_move(&player.y, &player.Vy);
            line_draw(player.x, player.y);

            bot_move(&bot.y, &bot.Vy, dot.y);
            line_draw(bot.x, bot.y);

            collide(dot.x, dot.y, player.y, bot.y, &dot.Vx, &score);
            if (dot.x < -3) {
                action = 5;
            }
            if (dot.x > 133) {
                action = 4;
            }

            oled.home();
            oled.setScale(1);
            oled.print(score);
            if (EEPROM.read(PINGPONG_RECORD) < 10) {
                oled.setCursor(122, 0);
            } else if (EEPROM.read(PINGPONG_RECORD) >= 10) {
                oled.setCursor(117, 0);
            } else if (EEPROM.read(PINGPONG_RECORD) >= 100) {
                oled.setCursor(112, 0);
            }     
            oled.print(EEPROM.read(PINGPONG_RECORD));
        }

        if (action == 4) {
            if (score > EEPROM.read(PINGPONG_RECORD)) {
                EEPROM.update(PINGPONG_RECORD, score);
            }
            oled.clear();
            oled.setScale(2);
            oled.setCursor(0, 3);
            oled.print("Ð¢Ñ Ð¿Ð¾Ð±ÐµÐ´Ð¸Ð»!");
            oled.update();
            delay(3000);
            action = 1;
        }

        if (action == 5) {
            if (score > EEPROM.read(PINGPONG_RECORD)) {
                EEPROM.update(PINGPONG_RECORD, score);
            }
            Serial.println(EEPROM.read(PINGPONG_RECORD));
            oled.clear();
            oled.setScale(2);
            oled.setCursorXY(48, 17);
            oled.print("Ð¢Ñ");
            oled.setCursorXY(0, 32);
            oled.println("Ð¿ÑÐ¾Ð¸Ð³ÑÐ°Ð» :(");
            oled.update();
            delay(3000);
            action = 1;
        }
        oled.update();
    }
}


void line_draw(int x, int y) {
    oled.rect(x, y, x + 1, y + 12, OLED_FILL);
}

void dot_draw(int x, int y) {
    oled.rect(x, y, x + 3, y + 3, OLED_FILL);
}

void collide(int dot_x, int dot_y, int player_y, int bot_y, int8_t* dot_Vx, byte* score) {
    if (((dot_x < 15) && (dot_x > 11)) && (((dot_y > player_y) && (dot_y < player_y + 12)) || ((dot_y + 3 > player_y) && (dot_y + 3 < player_y + 12)))) {
        *dot_Vx = -*dot_Vx;
        *score = *score + 1;
    }
    if (((dot_x > 110) && (dot_x < 113)) && (((dot_y > bot_y) && (dot_y < bot_y + 12)) || ((dot_y + 3 > bot_y) && (dot_y + 3 < bot_y + 12)))) {
        *dot_Vx = -*dot_Vx;
    }
}

void player_move(int* y, int8_t* Vy) {
    if (joy_y == 1) {
        *Vy = 0;
    } else if (joy_y < -200) {
        *Vy = 3;
    } else if (joy_y < -100) {
        *Vy = 2;
    } else if (joy_y < 1) {
        *Vy = 1;
    } else if (joy_y > 200) {
        *Vy = -3;
    } else if (joy_y > 100) {
        *Vy = -2;
    } else if (joy_y > 1) {
        *Vy = -1;
    }
    *y += *Vy;
    if (*y >= 51) {
        *y = 51;
    } else if (*y <= 1) {
        *y = 1;
    }
}

void bot_move(int* y, int8_t* Vy, int dot_y) {
    *y = constrain(*y, 1, 49);

    if (dot_y > *y + 6) {
        *y += *Vy;
    } else if (dot_y < *y + 6) {
        *y -= *Vy;
    }
}

void dot_move(int* dot_x, int* dot_y, int8_t* dot_Vx, int8_t* dot_Vy) {
    *dot_x += *dot_Vx;
    *dot_y += *dot_Vy;

    if (*dot_y < 2) {
        *dot_Vy = -*dot_Vy;
    }
    if (*dot_y > 58) {
        *dot_Vy = -*dot_Vy;
    }
}

void randspeed(int8_t* dot_Vx, int8_t* dot_Vy) {
    while (*dot_Vx == 0) {
        *dot_Vx = (random(-3, -1));
    }
    while (*dot_Vy == 0) {
        *dot_Vy = (random(-3, 3));
    }
}
