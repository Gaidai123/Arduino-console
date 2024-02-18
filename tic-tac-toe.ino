void tic_tac_toe_play() {
    uint32_t move_timer;
    const byte shapes_x[3] = { 34, 56, 77 };
    const byte shapes_y[3] = { 2, 24, 45 };
    byte x = 1;
    byte y = 1;
    byte step = 1; // 1 - ÐºÑÐµÑÑÐ¸Ðº, 2 - Ð½Ð¾Ð»Ð¸Ðº
    byte rotate = NONE;
    byte winner = 0; // 1 - ÐºÑÐµÑÑÐ¸Ðº, 2 - Ð½Ð¾Ð»Ð¸Ðº, 3 - Ð½Ð¸ÑÑÑ
    byte game_field[][3] = {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
    };  // 0 - Ð½Ð¸ÑÐµÐ³Ð¾, 1 - ÐºÑÐµÑÑÐ¸Ðº, 2 - Ð½Ð¾Ð»Ð¸Ðº

    while (1) {
        if ((millis() - move_timer) >= 200) {
            move_timer = millis();
            oled.clear();
#if (DEBUG == 2)
            Serial.print(rotate);
            Serial.print(", ");
            Serial.print(x);
            Serial.print(", ");
            Serial.print(y);
            Serial.print("x = ");
            Serial.print(joy_x);
            Serial.print(", y = ");
            Serial.println(joy_y);
s#endif
            rotate = find_rotate(rotate);
            curs_move(&x, &y, &rotate, shapes_x, shapes_y);
            curs_draw(shapes_x[x], shapes_y[y]);
            click(x, y, game_field, &step);
            field_draw(game_field, shapes_x, shapes_y, step);
            winner = is_win(game_field);
            if (winner != 0) {
                tic_tac_game_over(winner);
                break;
            }

            oled.update();
        }

        if (jx.tick()) {
            joy_y = -jx.value();
        }
        if (jy.tick()) {
            joy_x = jy.value();
        }
    }
}
 
byte is_win(byte game_field[][3]) {
    if ((game_field[0][0] == 1 && game_field[0][1] == 1 && game_field[0][2] == 1) ||
        (game_field[1][0] == 1 && game_field[1][1] == 1 && game_field[1][2] == 1) ||
        (game_field[2][0] == 1 && game_field[2][1] == 1 && game_field[2][2] == 1) ||
        (game_field[0][0] == 1 && game_field[1][0] == 1 && game_field[2][0] == 1) ||
        (game_field[0][1] == 1 && game_field[1][1] == 1 && game_field[2][1] == 1) ||
        (game_field[0][2] == 1 && game_field[1][2] == 1 && game_field[2][2] == 1) ||
        (game_field[0][0] == 1 && game_field[1][1] == 1 && game_field[2][2] == 1) ||
        (game_field[0][2] == 1 && game_field[1][1] == 1 && game_field[2][0] == 1)) 
        {
            return 1;
            
    } else if (
        (game_field[0][0] == 2 && game_field[0][1] == 2 && game_field[0][2] == 2) ||
        (game_field[1][0] == 2 && game_field[1][1] == 2 && game_field[1][2] == 2) ||
        (game_field[2][0] == 2 && game_field[2][1] == 2 && game_field[2][2] == 2) ||
        (game_field[0][0] == 2 && game_field[1][0] == 2 && game_field[2][0] == 2) ||
        (game_field[0][1] == 2 && game_field[1][1] == 2 && game_field[2][1] == 2) ||
        (game_field[0][2] == 2 && game_field[1][2] == 2 && game_field[2][2] == 2) ||
        (game_field[0][0] == 2 && game_field[1][1] == 2 && game_field[2][2] == 2) ||
        (game_field[0][2] == 2 && game_field[1][1] == 2 && game_field[2][0] == 2)) 
        {
            return 2;
    } else if (
        game_field[0][0] != 0 && game_field[0][1] != 0 && game_field[0][2] != 0 &&
        game_field[1][0] != 0 && game_field[1][1] != 0 && game_field[1][2] != 0 &&
        game_field[2][0] != 0 && game_field[2][1] != 0 && game_field[2][2] != 0) 
        {
            return 3;
        }
    return 0;
}

void tic_tac_game_over(byte winner) {
    switch(winner) {
        case 1:
            oled.clear();
            oled.setScale(2);
            oled.setCursorXY(18, 17);
            oled.print("ÐÑÐµÑÑÐ¸ÐºÐ¸");
            oled.setCursorXY(15, 34);
            oled.println("Ð¿Ð¾Ð±ÐµÐ´Ð¸Ð»Ð¸!");
            oled.update();
            delay(1200);
            break;
        case 2: 
            oled.clear();
            oled.setScale(2);
            oled.setCursorXY(30, 17);
            oled.print("ÐÐ¾Ð»Ð¸ÐºÐ¸");
            oled.setCursorXY(15, 34);
            oled.println("Ð¿Ð¾Ð±ÐµÐ´Ð¸Ð»Ð¸!");
            oled.update();
            delay(1200);
            break;
        case 3:
            oled.clear();
            oled.setScale(2);
            oled.setCursorXY(34, 26);
            oled.print("ÐÐ¸ÑÑÑ!");
            oled.update();
            delay(1200);
            break;
    }
}

void click(byte x, byte y, byte game_field[][3], byte* step) {
    if ((!digitalRead(joy_button) == 1) && (game_field[x][y] == 0)) {
        game_field[x][y] = *step;
        if (*step == 1) {
            *step = 2;
        } else {
            *step = 1;
        }
    }
}

void curs_move(byte* x, byte* y, byte* rotate, byte shapes_x[], byte shapes_y[]) {
    switch (*rotate) {
        case UP:
            *y -= 1;
            break;
        case RIGHT:
            *x += 1;
            break;
        case DOWN:
            *y += 1;
            break;
        case LEFT:
            *x -= 1;
            break;
    }
    *x = constrain(*x, 0, 2);
    *y = constrain(*y, 0, 2);
    *rotate = NONE;
}

void curs_draw(byte x, byte y) {
    if (x == 56 || x == 77) {
        x--;
    }
    if (y != 2) {
        oled.line(x + 19, y + 18, x, y + 18);
    } else {
        oled.line(x + 19, y + 19, x, y + 19);
    }
    oled.line(x, y - 1, x, y + 19);
    oled.line(x, y - 1, x + 19, y - 1);
    oled.line(x + 19, y - 1, x + 19, y + 19);
}

void field_draw(byte game_field[][3], byte shapes_x[], byte shapes_y[], byte step) {  //step = 2 - Ð½Ð¾Ð»Ð¸Ðº, step = 1 - ÐºÑÐµÑÑ
    oled.line(54, 0, 54, 64, OLED_FILL);
    oled.line(75, 0, 75, 64, OLED_FILL);
    oled.line(33, 22, 95, 22, OLED_FILL);
    oled.line(33, 43, 95, 43, OLED_FILL);

    oled.line(6, 22, 28, 22);
    oled.line(6, 22, 6, 44);
    oled.line(6, 44, 28, 44);
    oled.line(28, 22, 28, 44);

    if (step == 1) {
        cross_draw(8, 24);
    } else {
        circle_draw(8, 24);
    }

    byte i = 0;
    byte j = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (game_field[i][j] == 1) {
                cross_draw(shapes_x[i], shapes_y[j]);
            } else if (game_field[i][j] == 2) {
                circle_draw(shapes_x[i], shapes_y[j]);
            }
        }
    }
}

void cross_draw(byte x, byte y) {
    oled.line(x + 0, y + 0, x + 17, y + 17);
    oled.line(x + 17, y + 0, x + 0, y + 17);
}

void circle_draw(byte x, byte y) {
    oled.circle(x + 9, y + 9, 8.5, OLED_STROKE);
}