#include "doctest.h"
#include "game.hpp"
#include "model-constants.hpp"
#include "player.hpp"

TEST_CASE("In-game pointers") {
    Platformer::Game game("../model/empty.json");
    Platformer::Player *player = game.getPlayer();
    CHECK((player->getGame() == &game));
    auto &observers = player->getObservers();
    for (auto &observer : observers) {
        CHECK((observer->getPlayer() == player));
    }
}

void checkFreeJump(Platformer::Player &player, Platformer::Game &game) {
    auto initialPos = player.getPos();
    player.jump();
    game.update();
    int frame = 1;
    CHECK((player.getPos().y < initialPos.y));
    while (player.getPos().y < initialPos.y) {
        CHECK((player.getPos().x == initialPos.x));
        CHECK(
            (player.getPos().y ==
             initialPos.y -
                 frame *
                     (2 * Platformer::PLAYER_SPEED -
                      (frame - 1) * Platformer::FREEFALL_ACCELERATION) /
                     2)
        );
        frame++;
        game.update();
    }
}

void checkClimbLadder(Platformer::Player &player, Platformer::Game &game) {
    auto initialPos = player.getPos();
    auto prevPos = initialPos;
    player.jump();
    game.update();
    auto curPos = player.getPos();
    while (prevPos.y - curPos.y == Platformer::PLAYER_SPEED) {
        CHECK((curPos.x == initialPos.x));
        player.jump();
        game.update();
        prevPos = curPos;
        curPos = player.getPos();
        CHECK((curPos.y >= 0));
    }
    CHECK((curPos.y <= Platformer::BLOCK_SIZE / 2));
    int step = 0;
    while (player.getPos().y < initialPos.y) {
        player.moveDown();
        game.update();
        step++;
        CHECK((step < 1000));
    }
}

void moveHorizontally(
    Platformer::Player &player,
    Platformer::Game &game,
    int delta,
    bool checkY = true
) {
    auto initialPos = player.getPos();
    auto finishX = initialPos.x + delta;
    int step = 0;
    while ((player.getPos().x - finishX) * delta < 0) {
        if (delta > 0) {
            player.moveRight();
            step++;
            game.update();
            CHECK(
                (player.getPos().x ==
                 initialPos.x + step * Platformer::PLAYER_SPEED)
            );
        } else {
            player.moveLeft();
            step++;
            game.update();
            CHECK(
                (player.getPos().x ==
                 initialPos.x - step * Platformer::PLAYER_SPEED)
            );
        }
        if (checkY) {
            CHECK((player.getPos().y == initialPos.y));
        }
    }
}

void wait(Platformer::Game &game, int ticks) {
    for (int i = 0; i < ticks; i++) {
        game.update();
    }
}

TEST_CASE("Move") {
    Platformer::Game game("../tests/levels/move-test.json");
    Platformer::Player *player = game.getPlayer();
    auto initialPos = player->getPos();

    SUBCASE("Go left and right") {
        moveHorizontally(*player, game, -Platformer::BLOCK_SIZE);
        moveHorizontally(*player, game, Platformer::BLOCK_SIZE);
    }

    SUBCASE("Jump") {
        checkFreeJump(*player, game);
    }

    SUBCASE("Jump and hit block") {
        int step = 0;
        while (player->getPos().x > 4.5 * Platformer::BLOCK_SIZE) {
            player->moveLeft();
            game.update();
            step++;
            CHECK(
                (player->getPos().x ==
                 initialPos.x - Platformer::PLAYER_SPEED * step)
            );
            if (player->getPos().x - player->getSize().x / 2 <
                    6 * Platformer::BLOCK_SIZE &&
                player->getPos().x + player->getSize().x / 2 >
                    5 * Platformer::BLOCK_SIZE) {
                player->jump();
                game.update();
                while (player->getPos().y < initialPos.y) {
                    CHECK(
                        (player->getPos().y - player->getSize().y / 2 >=
                         2 * Platformer::BLOCK_SIZE)
                    );
                    game.update();
                }
            } else {
                checkFreeJump(*player, game);
            }
        }
    }

    SUBCASE("Fall out of the world") {
        moveHorizontally(
            *player, game, Platformer::BLOCK_SIZE / 2 + player->getSize().x / 2,
            false
        );
        wait(game, 600);
        CHECK((player->getPos() == initialPos));
    }
}

TEST_CASE("Ladder") {
    Platformer::Game game("../tests/levels/ladder-test.json");
    Platformer::Player *player = game.getPlayer();
    auto initialPos = player->getPos();

    SUBCASE("Simple ladder") {
        checkClimbLadder(*player, game);
    }

    SUBCASE("Ladder edges") {
        int step = 0;
        while (player->getPos().x < 5.5 * Platformer::BLOCK_SIZE) {
            player->moveRight();
            game.update();
            step++;
            CHECK(
                (player->getPos().x ==
                 initialPos.x + Platformer::PLAYER_SPEED * step)
            );
            if (player->getPos().x - player->getSize().x / 2 <
                5 * Platformer::BLOCK_SIZE) {
                checkClimbLadder(*player, game);
            } else {
                checkFreeJump(*player, game);
            }
        }
        player->reset();
        step = 0;
        while (player->getPos().x > 3.5 * Platformer::BLOCK_SIZE) {
            player->moveLeft();
            game.update();
            step++;
            CHECK(
                (player->getPos().x ==
                 initialPos.x - Platformer::PLAYER_SPEED * step)
            );
            if (player->getPos().x + player->getSize().x / 2 >
                4 * Platformer::BLOCK_SIZE) {
                checkClimbLadder(*player, game);
            } else {
                checkFreeJump(*player, game);
            }
        }
    }

    SUBCASE("Jump to ladder") {
        int step = 0;
        while (player->getPos().x > 2.5 * Platformer::BLOCK_SIZE) {
            player->moveLeft();
            game.update();
            step++;
            CHECK(
                (player->getPos().x ==
                 initialPos.x - Platformer::PLAYER_SPEED * step)
            );
        }
        player->jump();
        wait(game, 60);
        CHECK((player->getPos().y < initialPos.y));
        auto oldPos = player->getPos();
        game.update();
        CHECK((player->getPos() == oldPos));
    }

    SUBCASE("Cannot jump 2 blocks to ladder") {
        moveHorizontally(*player, game, Platformer::BLOCK_SIZE * 2);
        player->jump();
        wait(game, 120);
        CHECK((player->getPos().y == initialPos.y));
    }

    SUBCASE("Can stand on ladder") {
        moveHorizontally(*player, game, -Platformer::BLOCK_SIZE * 4);
    }
}

TEST_CASE("Killer") {
    Platformer::Game game("../tests/levels/killer-test.json");
    Platformer::Player *player = game.getPlayer();
    auto initialPos = player->getPos();

    SUBCASE("Killer on the floor") {
        moveHorizontally(
            *player, game,
            -(Platformer::BLOCK_SIZE / 2 - Platformer::PLAYER_SPEED -
              player->getSize().x / 2)
        );
        CHECK(!player->isDying());
        CHECK((player->getPos() != initialPos));
        player->moveLeft();
        CHECK(player->isDying());
        game.update();
        CHECK((player->getPos() == initialPos));
    }

    SUBCASE("Killer to the right") {
        moveHorizontally(
            *player, game,
            static_cast<int>(2.5 * Platformer::BLOCK_SIZE) -
                Platformer::PLAYER_SPEED - player->getSize().x / 2
        );
        CHECK(!player->isDying());
        CHECK((player->getPos() != initialPos));
        player->moveRight();
        CHECK(player->isDying());
        game.update();
        CHECK((player->getPos() == initialPos));
    }

    SUBCASE("Killer on the ceiling") {
        moveHorizontally(*player, game, Platformer::BLOCK_SIZE);
        player->jump();
        CHECK((player->getPos() != initialPos));
        wait(game, 60);
        CHECK((player->getPos() == initialPos));
    }

    SUBCASE("Killer to the left") {
        player->jump();
        game.update();
        moveHorizontally(
            *player, game,
            -(static_cast<int>(2.5 * Platformer::BLOCK_SIZE) -
              Platformer::PLAYER_SPEED - player->getSize().x / 2),
            false
        );
        CHECK(!player->isDying());
        CHECK((player->getPos() != initialPos));
        player->moveLeft();
        CHECK(player->isDying());
        game.update();
        CHECK((player->getPos() == initialPos));
    }
}

TEST_CASE("Trampoline") {
    Platformer::Game game("../tests/levels/tramp-test.json");
    Platformer::Player *player = game.getPlayer();
    auto initialPos = player->getPos();

    for (auto delta = -Platformer::BLOCK_SIZE; delta < Platformer::BLOCK_SIZE;
         delta += Platformer::PLAYER_SPEED) {
        moveHorizontally(*player, game, delta, false);
        while (player->getPos().y <
               3 * Platformer::BLOCK_SIZE - player->getSize().y / 2) {
            game.update();
        }
        game.update();
        if (player->getPos().x + player->getSize().x / 2 >=
                3 * Platformer::BLOCK_SIZE &&
            player->getPos().x - player->getSize().x / 2 <=
                4 * Platformer::BLOCK_SIZE) {
            CHECK(
                (player->getPos().y <
                 3 * Platformer::BLOCK_SIZE - player->getSize().y / 2)
            );
        } else {
            CHECK(
                (player->getPos().y >
                 3 * Platformer::BLOCK_SIZE - player->getSize().y / 2)
            );
        }
        player->reset();
    }
}
