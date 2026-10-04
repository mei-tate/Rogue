#include "rogue.h"

/* Defense is a finite buffer: damage consumes it before reducing health. */
static void applyDamageToPlayer(Player *player, int damage) {
    int absorbed;
    if (damage <= 0 || player == NULL || !player->alive) return;
    absorbed = player->defense < damage ? player->defense : damage;
    player->defense -= absorbed;
    player->health -= damage - absorbed;
    if (player->health <= 0) {
        player->health = 0;
        player->alive = 0;
    }
}

static void applyDamageToMonster(Monster *monster, int damage) {
    int absorbed;
    if (damage <= 0 || monster == NULL || !monster->alive) return;
    absorbed = monster->defense < damage ? monster->defense : damage;
    monster->defense -= absorbed;
    monster->health -= damage - absorbed;
    if (monster->health <= 0) {
        monster->health = 0;
        killMonster(monster);
    }
}

/* Faster combatant attacks first; ties go to the player. A defeated fighter
 * never gets a retaliation, regardless of initiative. */
int combat(Player *player, Monster *monster) {
    if (player == NULL || monster == NULL || !player->alive || !monster->alive)
        return 0;

    if (monster->speed > player->speed) {
        applyDamageToPlayer(player, monster->attack);
        if (player->alive) applyDamageToMonster(monster, player->attack);
    } else {
        applyDamageToMonster(monster, player->attack);
        if (monster->alive) applyDamageToPlayer(player, monster->attack);
    }

    return 1;
}
