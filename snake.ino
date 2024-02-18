void snake_play(byte* game) {
    uint32_t move_timer;
    byte score = 0;
    byte rotate = RIGHT;
    byte lenght = 4;
    byte x[16] = { 64, 60, 56, 52, -4, -4, -4, -4 };
    byte y[16] = { 36, 36, 36, 32, -4, -4, -4, -4 };
    byte apple_x = 96;
    byte apple_y = 36;
    byte snake_collide = 0;

    while (*game == SNAKE) {
        if (millis() - move_timer >= 250) {
            move_timer = millis();
            oled.clear();

            snake_move(x, y, lenght, rotate);
            snake_collide = is_snake_collide(x, y, lenght);
            if (is_snake_collide(x, y, lenght)) {
                game_over(game, score);
            }
            snake_draw(x, y, lenght);

            apple_collide(&apple_x, &apple_y, x, y, &lenght);
            apple_draw(apple_x, apple_y);

            score_draw(&score, lenght);
            
            #if (DEBUG == 1)
            Serial.print(apple_x);
            Serial.print(", ");
            Serial.print(apple_y);
            Serial.print(", ");
            Serial.print(x[0]);
            Serial.print(", ");
            Serial.print(y[0]);
            Serial.print(", ");
            Serial.println(snake_collide);
            #endif

            oled.update();
        }

        if (jx.tick()) {
            joy_x = jx.value();
        }
        if (jy.tick()) {
            joy_y = jy.value();
        }

        rotate = find_rotate(rotate);
    }
}

void snake_draw(byte x[], byte y[], byte lenght) {
    snakehead_draw(x[0], y[0]);
    byte i;
    for (i = 1; i < lenght; i++) {
        snakepart_draw(x[i], y[i]);
    }
}

void snakehead_draw(int x, int y) {
    oled.rect(x, y, x + 3, y + 3, OLED_FILL);
}

void snakepart_draw(int x, int y) {
    oled.rect(x + 1, y + 1, x + 2, y + 2, OLED_FILL);
    oled.line(x + 1, y, x + 2, y, OLED_FILL);
    oled.line(x + 3, y + 1, x + 3, y + 2, OLED_FILL);
    oled.line(x + 1, y + 3, x + 2, y + 3, OLED_FILL);
    oled.line(x, y + 1, x, y + 2, OLED_FILL);
}

void snake_move(byte x[], byte y[], byte lenght, byte rotate) {
    byte i;
    for (i = lenght - 1; i > 1; i--) {
        x[i] = x[i - 1];
        y[i] = y[i - 1];
    }

    x[1] = x[0];
    y[1] = y[0];

    switch (rotate) {
        case UP:
            y[0] = y[0] - 4;
            break;
        case RIGHT:
            x[0] = x[0] + 4;
            break;
        case DOWN:
            y[0] = y[0] + 4;
            break;
        case LEFT:
            x[0] = x[0] - 4;
            break;
    }
}

void apple_random(byte* apple_x, byte* apple_y) {
    *apple_x = random(0, 31) * 4;
    *apple_y = random(0, 12) * 4;
}

void apple_draw(byte x, byte y) {
    oled.rect(x + 1, y + 1, x + 2, y + 2, OLED_FILL);
    oled.line(x + 1, y, x + 2, y, OLED_FILL);
    oled.line(x + 3, y + 1, x + 3, y + 2, OLED_FILL);
    oled.line(x + 1, y + 3, x + 2, y + 3, OLED_FILL);
    oled.line(x, y + 1, x, y + 2, OLED_FILL);
}

void apple_collide(byte* apple_x, byte* apple_y, byte x[], byte y[], byte* lenght) {
    if ((*apple_x == x[0]) && (*apple_y == y[0])) {
        randomSeed(micros());
        apple_random(apple_x, apple_y);
        *lenght += 1;
    }
}

byte is_snake_collide(byte x[], byte y[], byte lenght) {
    byte i;
    for (i = 1; i < lenght; i++) {
        if ((x[i] == x[0]) && (y[i] == y[0])) {
            return 1;
        }
    }
    if ((x[0] > 124) || (x[0] < 0)) {
        return 1;
    }
    if ((y[0] > 124) || (y[0] < 0)) {
        return 1;
    }
    return 0;
}

void score_draw(byte* score, byte lenght) {
    *score = lenght - 4;
    oled.home();
    oled.setScale(1);
    oled.print(*score);
    if (EEPROM.read(SNAKE_RECORD) < 10) {
        oled.setCursor(122, 0);
    } else if (EEPROM.read(SNAKE_RECORD) >= 10) {
        oled.setCursor(117, 0);
    } else if (EEPROM.read(SNAKE_RECORD) >= 100) {
        oled.setCursor(112, 0);
    }
    oled.print(EEPROM.read(SNAKE_RECORD));
}

void game_over(byte* game, byte score) {
    oled.clear();
    oled.setScale(2);
    oled.setCursorXY(48, 17);
    oled.print("Ð¢Ñ");
    oled.setCursorXY(0, 32);
    oled.println("Ð¿ÑÐ¾Ð¸Ð³ÑÐ°Ð» :(");
    oled.update();

    if (score > EEPROM.read(SNAKE_RECORD)) {
        EEPROM.update(SNAKE_RECORD, score);
    }

    delay(2000);
    *game = MENU;
}

byte find_rotate(byte rotate) {
    if (joy_y > 1 && joy_x > 1) {
        if ((joy_y > joy_x) && rotate != DOWN) {
            return UP;
        } else if ((joy_y < joy_x) && rotate != LEFT) {
            return RIGHT;
        }
    }
    if (joy_y < 1 && joy_x > 1) {
        if ((-joy_y > joy_x) && rotate != UP) {
            return DOWN;
        } else if ((-joy_y < joy_x) && rotate != LEFT) {
            return RIGHT;
        }
    }
    if (joy_y < 1 && joy_x < 1) {
        if ((joy_y < joy_x) && rotate != UP) {
            return DOWN;
        } else if ((joy_y > joy_x) && rotate != RIGHT) {
            return LEFT;
        }
    }
    if (joy_y > 1 && joy_x < 1) {
        if ((joy_y > -joy_x) && rotate != DOWN) {
            return UP;
        } else if ((joy_y < -joy_x) && rotate != RIGHT) {
            return LEFT;
        }
    }
    return rotate;
}