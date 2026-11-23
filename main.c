/*
 * GRAU 360 - jogo simples de grau para Xbox 360 (libxenon)
 *
 * A ou RT = acelera (levanta a roda)
 * B ou LT = freia (abaixa a roda)
 * START   = comecar / jogar de novo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xenos/xenos.h>
#include <console/console.h>
#include <usb/usbmain.h>
#include <input/input.h>
#include <time/time.h>
#include <xenon_soc/xenon_power.h>

#define LARGURA 50

enum { MENU, JOGANDO, FIM };

/* Desenha a barra do equilibrio. O = posicao da moto, ==== = zona perfeita */
static void desenha_barra(float angulo)
{
    char barra[LARGURA + 1];
    int i, pos;

    for (i = 0; i < LARGURA; i++) {
        int a = i * 100 / (LARGURA - 1);
        barra[i] = (a >= 40 && a <= 60) ? '=' : '-';
    }
    barra[LARGURA] = '\0';

    pos = (int)(angulo * (LARGURA - 1) / 100.0f);
    if (pos < 0) pos = 0;
    if (pos > LARGURA - 1) pos = LARGURA - 1;
    barra[pos] = 'O';

    printf("  CHAO [%s] CAPOTOU\n", barra);
}

int main(void)
{
    static struct controller_data_s c;
    int estado = MENU;
    float angulo = 0.0f, vel = 0.0f;
    int pontos = 0, recorde = 0, levantou = 0;
    const char *motivo = "";

    xenon_make_it_faster(XENON_SPEED_FULL);
    xenos_init(VIDEO_MODE_AUTO);
    console_init();
    usb_init();
    usb_do_poll();

    while (1) {
        int gas, freio;

        usb_do_poll();
        get_controller_data(&c, 0);

        gas = c.rt;
        if (c.a) gas = 255;
        freio = c.lt;
        if (c.b) freio = 255;

        /* ---------- logica ---------- */
        if (estado == JOGANDO) {
            vel += (gas - freio) / 255.0f * 0.35f;   /* gas levanta, freio abaixa */
            vel += (angulo - 50.0f) * 0.004f;        /* passou do meio: tende a cair */
            if (levantou)
                vel += ((rand() % 100) - 50) / 50.0f * 0.12f; /* vento aleatorio */
            vel *= 0.92f;
            angulo += vel;

            if (angulo < 0.0f) {
                angulo = 0.0f;
                if (!levantou) vel = 0.0f;
            }
            if (angulo >= 20.0f) levantou = 1;

            if (levantou)
                pontos += (angulo >= 40.0f && angulo <= 60.0f) ? 3 : 1;

            if (angulo >= 100.0f) {
                motivo = "CAPOTOU! Voce passou do ponto.";
                estado = FIM;
            } else if (levantou && angulo <= 0.0f) {
                motivo = "A roda voltou pro chao.";
                estado = FIM;
            }
            if (estado == FIM && pontos > recorde)
                recorde = pontos;
        }

        /* ---------- tela ---------- */
        console_clrscr();
        printf("\n  ===== GRAU 360 =====\n\n");

        if (estado == MENU) {
            printf("  Segure o grau o maximo que conseguir!\n\n");
            printf("  A ou RT .... acelera (levanta a roda)\n");
            printf("  B ou LT .... freia (abaixa a roda)\n\n");
            printf("  Fique na zona '=' para ganhar mais pontos.\n");
            printf("  Se passar do limite, capota. Se cair, acaba.\n\n");
            printf("  Recorde: %d\n\n", recorde);
            printf("  Aperte START para comecar\n");
            if (c.start) {
                angulo = 0.0f; vel = 0.0f; pontos = 0; levantou = 0;
                estado = JOGANDO;
            }
        } else if (estado == JOGANDO) {
            printf("  Pontos: %d     Recorde: %d\n\n", pontos, recorde);
            desenha_barra(angulo);
            printf("\n");
            if (!levantou)
                printf("  Acelere para levantar a roda!\n");
            else
                printf("  Mantenha o O na zona '='!\n");
        } else { /* FIM */
            printf("  %s\n\n", motivo);
            printf("  Pontos: %d\n", pontos);
            printf("  Recorde: %d\n\n", recorde);
            printf("  Aperte START para jogar de novo\n");
            if (c.start) {
                angulo = 0.0f; vel = 0.0f; pontos = 0; levantou = 0;
                estado = JOGANDO;
            }
        }

        mdelay(40);
    }

    return 0;
}
