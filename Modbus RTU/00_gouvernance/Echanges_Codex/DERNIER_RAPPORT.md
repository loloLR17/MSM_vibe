---
mission_id: TR2-20261011-RS485-RX-BRINGUP-008
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 7ce6f894a21180d0420c7f0d2754bacdc05211ea
mission_sha: 616030bb6d9fa82828ac368b07c5a68e98d54665
result_sha: 0d4dde6cf31508ab54911f976f51f45c2f819152
created_at_utc: 2026-10-11T12:35:45.183442+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission008 — correction minimale du bring-up RX

**BLOCKED provisoire uniquement pour preuve de publication/relecture distante encore à obtenir.** Critères logiciels satisfaits : **COMPILÉ STM32 / NON FLASHÉ / NON QUALIFIÉ RX**. Aucune réception physique annoncée.

## Références et modifications

HEAD initial propre main `7ce6f894a21180d0420c7f0d2754bacdc05211ea`, fetch et pull fast-forward vers mission `616030bb6d9fa82828ac368b07c5a68e98d54665`. Contrat READY laissé inchangé. Son champ `base_sha` manque contrairement au protocole : rapport `null`, sans inventer une valeur ; HEAD initial et référence du contrat consignés. Permissions et périmètre explicites permettent de poursuivre sans ambiguïté opérationnelle.

AGENTS.md, protocole, journal, mission/rapport007, diagnostic007, préparation006, pinout et freezes, sources STM32, HAL/CMSIS et schéma officiel inspectés. Travaux préexistants des missions005/002 conservés dans leurs deux stashes. Archives007 copiées exactement de `7ce6f894a21180d0420c7f0d2754bacdc05211ea` avant remplacement du rapport ; aucune réécriture historique.

Commit code `0d4dde6cf31508ab54911f976f51f45c2f819152` : `STM32: hold PG4 DE low before enabling VDDIO2 at boot`.

- `05_Firmware/platform/stm32/stm32_rs485_rx_gpio.{c,h}` : horloges PWR/GPIOG, préchargement PG4 bas avant output push-pull/pull-down, puis HAL officielle EnableVddIO2. Aucun PG5 ni DE haut.
- `platform/stm32/main.c` : appel dès HAL_MspInit, donc avant qualifications de démarrage et avant configuration UART PG7/PG8 AF8. CMake STM32 ajoute ce seul source ; interfaces/cœur fonctionnel inchangés.
- `tests/platform/test_stm32_rs485_rx_gpio.c` et HAL mock isolée : vrai source de production, ordre et masques contrôlés, latch initial haut, IO2SV déjà actif/inactif. CMake hôte ajoute ce test sans modifier les tests antérieurs.
- `00_gouvernance/BRINGUP_RS485_RX_008.md` : preuves documentaires, limites et procédure utilisateur courte d'un seul essai futur ; `ETAT_COURANT_TR2.md` actualisé avec observations utilisateur008, séquence compilée, empreintes et conditions restantes. Mapping gelé, services et E4 inchangés.

## Preuves et limites électriques

Lecture graphique du [schéma Click v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf) : J2 relie DE à /RE ; J3 relie /RE à RST ; VIO reliée à VCC sélectionnée. La [table fabricant](https://www.mikroe.com/rs485-isolator-2-click) nomme JP2 le contrôle commun. Révision et continuité de la carte réelle NON VÉRIFIÉES. Pont J2 soudé, DE vers PG4, /RE et PG5 sans fil externe : observations utilisateur du contrat, pas observations Codex. Si J2 correspond et est fermé, DE bas permet /RE bas sans PG5. Vérifier absence de commande RST concurrente via J3 et tension VIO Click~3,31 V ; SB28 ne mesure pas VIO Click.

[ADM2867E table12](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf) : DE bas désactive émission, /RE bas active réception. Mesure utilisateur PG4=0,88 V ne démontre ni flottement ni bas stable ; à VIO3,31 V le seuil VIL nominal est 0,993 V, sans garantie de stabilité par cette seule mesure.

[AN5373 §2.1.2](https://www.st.com/resource/en/application_note/an5373-getting-started-with-stm32u5-mcu-hardware-development-stmicroelectronics.pdf) : IO2SV libère PG2..15 après présence du rail. Présence fondée sur SB28 et observations contrat008. Autres pins non configurées par ce correctif ; état physique non mesuré. Reset avant HAL_MspInit reste sans garantie matérielle DE bas ; pull-down logiciel intervient ensuite. Aucun autre écrivain PG4/IO2SV trouvé dans le code projet. Ce correctif n'active pas de TX fonctionnel ni serveur répondant.

## Validations réellement exécutées

Choix proportionné : plateforme STM32 seule, aucun changement fonctionnel ou interface partagée ; cross-build-only et tests ciblés GPIO/codec/réception. Pas de validation complète ou .NET sans motif de régression.

```sh
cmake -S 'Modbus RTU/05_Firmware' -B 'Modbus RTU/05_Firmware/build-host-validation'
cmake --build 'Modbus RTU/05_Firmware/build-host-validation' --target test_stm32_rs485_rx_gpio
ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' -R '^(stm32_rs485_rx_gpio|codec|p12d_rtu_receiver)$' --output-on-failure
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only
```

Configuration/build hôte réussis ; **3/3 tests ciblés passés** (105 tests enregistrés, pas tous exécutés). Cross-build ARM réussi, code retour0. Text157820/data124/bss129072 ; text+128 octets versus006/007, bss inchangé. Quatre avertissements newlib existants `_close/_lseek/_read/_write` non implémentés ; aucun masquage. `arm-none-eabi-objdump` et assertions Python sur les appels ELF confirment HAL_Init→HAL_MspInit→WritePin→GPIO_Init→EnableVddIO2, puis init transport GPIO AF→UART. Contrôle du code compilé, pas mesure de transitoire.

Artefacts locaux non suivis Git : `05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin` **157944 octets**, SHA256 `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b` ; `.elf` SHA256 `6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e`. Logs hors Git `/tmp/tr2-mission008-build.log`, `/tmp/tr2-mission008-host-config.log`, `/tmp/tr2-mission008-arm-order.log` et CTest LastTest.log. Le script n'est pas exécutable, d'où invocation bash, sans modification de permissions.

Lecture PDF : pip absent, roue PyMuPDF1.28.2 téléchargée/vérifiée contre empreinte PyPI puis extraite uniquement sous /tmp pour rendre le schéma officiel ; aucune installation globale ni dépendance du firmware ajoutée. Vérification d'inventaire des fichiers : première assertion attendait12 au lieu de11, arrêt avant indexation, recompte11 puis contrôle réussi ; aucun changement de code requis.

`git diff --check`, inspection diff/index, recherche de secrets et destination HTTPS main réalisées. Aucun effacement/reset Git, aucune modification de configuration Codex ; stashes conservés. Permissions exercées : modification, compilation/tests, commit ; push normal prévu par le contrat. **Aucun flash, debug cible, OpenOCD/GDB, émission COM7, reset, écriture registre ou action physique.**

## Procédure future et transmission à ChatGPT

[Procédure courte complète](../BRINGUP_RS485_RX_008.md) : utilisateur vérifie J2/révision, VIO3,31 V et /RE bas ; coordonne propriétaire ST-LINK (GDB fermé rapporté, OpenOCD potentiellement actif), garde microSD absente. Après permissions distinctes flash/debug et procédure qualifiée, programme/vérifie BIN008 et identité Flash avant symboles ELF008. Snapshot AVANT, reprise cible, **un seul** envoi HP COM7 1152008E1 `01 03 03 E8 00 01 04 7A`, attente1s, snapshot APRÈS : +8 octets, +1 ADU valide exacte, zéro erreur UART/CRC/overflow, aucune réponse attendue. Aucun essai exécuté en008. Ne pas réutiliser les empreintes006 ni les symboles d'un ELF différent de Flash.

Journal actualisé : observations de banc rapportées, rôle documentaire J2/VIO nouvellement vérifié, correctif et preuves logicielles, préparation future. À ChatGPT : revue indépendante du résultat distant, puis décision explicite sur qualification physique et conditions de sécurité reset. RX physique et dette E4 restent ouverts.

## Publication

Rapport provisoire ; preuve distante à compléter selon protocole V2-B3.
