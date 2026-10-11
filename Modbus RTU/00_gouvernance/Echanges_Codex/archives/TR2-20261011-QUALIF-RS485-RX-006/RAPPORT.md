---
mission_id: TR2-20261011-QUALIF-RS485-RX-006
status: DONE
base_ref: main
base_sha: 9753b3d6b46243f0472a0d33c5fa82b509dcea71
initial_head: 9753b3d6b46243f0472a0d33c5fa82b509dcea71
result_sha: 330a6393572b4478c079d23829cda1b952672103
created_at_utc: 2026-10-11T11:41:59Z
updated_at_utc: 2026-10-11T11:42:28Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 006 — préparation instrumentée du premier essai RS-485 RX

**DONE pour la préparation logicielle : publication et relecture distante intégrale établies au SHA `41ed8c02fee26989d2e2a67cd52dff53b4afe795`.**
Livrables logiciels réalisés ; **VALIDÉ hôte ciblé / COMPILÉ STM32 / PRÉPARÉ essai / NON QUALIFIÉ physique**.
Décision : **NON PRÊT pour essai physique immédiat**, conditions explicites ci-dessous.
La mission préparatoire est clôturée ; elle ne qualifie pas la liaison.

## État réel et modifications

HEAD initial main propre `9753b3d6b46243f0472a0d33c5fa82b509dcea71`, origin HTTPS vérifiée.
Fetch puis pull fast-forward sans risque vers `c8e2b5c3f112a272912cf260c6c8a94d148ab418` ; base du contrat présente dans l'historique.
AGENTS.md, protocole V2-B3, mission 006, journal intégral, mission/rapport 005, pinout, préparation 005 et sources réellement inspectés.
Aucun merge/rebase/reset ; stashes 005 et 002 conservés. Mission ChatGPT READY inchangée.

Commit évalué `330a6393572b4478c079d23829cda1b952672103` — `STM32: instrument receive-only RS485 probe and prepare mission 006`.
Sept fichiers :

- `05_Firmware/platform/stm32/main.c` : quatre diagnostics supplémentaires uniquement dans le harness RX : ready après start réussi, polling, erreurs CRC distinctes, copie de la dernière ADU (256 octets). Compteurs existants conservés, aucun changement du cœur ou des interfaces. Snapshot à prendre hors réception ; les octets au-delà de la longueur courante restent anciens.
- `05_Firmware/platform/host/rs485_rx_probe/Send-Tr2RxProbe.ps1` : hors ligne par défaut, CRC calculé et contrôles intégrés avant tout port ; COM7 115200 8E1, écriture binaire unique sous switch `-Transmit`, attente ~1 s, aucune retransmission. FC03 B1 registre1000 quantité1, unité1 marqueur uniquement, hex `01 03 03 E8 00 01 04 7A`. Une réponse absente est compatible avec le diagnostic RX.
- `05_Firmware/platform/host/rs485_rx_probe/rx_snapshot.gdb` : définition de macro de lecture à casts explicites pour ELF sans types DWARF ; aucun attach, flash, reset ou écriture lors du chargement.
- `00_gouvernance/PREPARATION_RS485_RX_006.md` : matrice de faits/documentation/hypothèses, prérequis électriques, firmware identifié par comparaison Flash/BIN, future voie de diagnostic à autoriser, trois étapes AVANT→un envoi HP→APRÈS, deltas et ADU exacte, diagnostic différentiel et limites.
- `00_gouvernance/ETAT_COURANT_TR2.md` actualisé : observations utilisateur du 11 octobre, préparation/validations réelles, politique Windows bloquant l'exécution du script, conditions physiques et prochaine mission.
- Deux archives 005 : copies immuables exactes de mission et rapport depuis `9753b3d...`, sans archive écrasée.

Production binding reste désactivé, UART et GPIO DE//RE inchangés, aucun TX ajouté.
Freezes et dette E4 microSD/power-loss inchangés.

## Validation réellement observée

```bash
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  bash ./tr2_validate.sh --cross-build-only
ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' \
  -R '^(codec|p12d_rtu_receiver)$' --output-on-failure
```

Cross-build retour **0**, compilation/link STM32U575 Cortex-M33 réussis.
**2/2 tests hôte ciblés réussis**, binaires existants de 005 : leurs sources et celles du cœur n'ont pas changé.
Validation proportionnée : changements C strictement dans le harness, aucun cœur/interface partagé modifié ; pas de nouvelle campagne hôte complète ni .NET.
Quatre avertissements newlib habituels `_close`, `_lseek`, `_read`, `_write`, aucune erreur de compilation/link finale.
ARM text157692 / data124 / bss129072 : bss +264 octets vs005 ; pas de mesure de marge dynamique MCU.

Autres contrôles effectivement réalisés, sans matériel :

- Générateur de production `tr2_rtu_request 1 b1` → `010303e80001047a` ; CRC indépendant Python résidu0, decode → `unit=1 pdu=0303e80001` ; CRC corrompu rejeté, retour1.
- Analyse syntaxique réelle via parseur AST Windows PowerShell : aucune erreur. **Exécution complète du script NON VÉRIFIÉE** : Windows T1000 refuse `-File` par politique d'exécution. Aucun `Bypass`, changement de politique ou exécution détournée. Test hors ligne sur HP requis avant émission ; si même refus, décision humaine nécessaire sur l'exécution du script.
- Chargement de la macro via `gdb-multiarch --batch -nx <ELF> -ex 'source .../rx_snapshot.gdb' -ex 'show user tr2-rx-snapshot'` réussi, **sans target**.
- `arm-none-eabi-nm -S` : diagnostics32 bits, tableau256 octets présents en RAM ; aucune hypothèse sur les symboles du firmware effectivement chargé.
- Diff/index examinés, `git diff --check` et `git diff --cached --check` réussis, contenus indexés comparés aux fichiers locaux, archives exactes et recherche de secrets sans occurrence. Aucun artefact build ou fichier hors périmètre indexé.

BIN157816 octets SHA256 `f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d`.
ELF SHA256 `5ac5f0c66645692ff78579d792227ceb834f331baef9b8ed3a64d0ea55c293c3`.
Artefacts/logs conservés hors Git dans build ignoré et `/tmp/tr2-mission006-*` ; ces fichiers temporaires ne constituent pas une archive durable publiée.

## Preuves et limites avant essai physique

| Niveau | État / preuve |
|---|---|
| Documentation et code | LPUART1 PG7/PG8, IRQ LPUART1/TIM6, 1152008E1, T1.5/T3.5 750/1750µs ; PG4/PG5 candidats seulement |
| SWD | Détection OpenOCD rapportée par utilisateur dans le contrat, aucune nouvelle session Codex |
| Instrumentation | Compilée et symboles inspectés, firmware actuellement chargé **non identifié** |
| Outil HP | Trame/CRC vérifiés hors ligne ; syntaxe PS vérifiée ; exécution complète et COM7 émission **non vérifiées** |
| RX UART/ADU | Aucun octet physique observé ; futurs deltas +8/+1/+1 et copie ADU exacte requis |
| Serveur/PDU/TX | Binding désactivé ; aucune réponse garantie, aucun serveur/TX qualifié |
| Électricité | Niveaux VIO/VDDIO2, RXD et DE//RE non observés ; état IO2SV à lire après autorisation |
| E4 | Dette ouverte ; présence microSD actuelle à confirmer avant tout boot/reset/flash |

Les données de banc du contrat sont **rapportées par l'utilisateur**, pas mesurées par Codex : FT232R COM7, montage ADM2867E/MIKROE3863, câblage déjà fait, rouge5V isolé, continuité A/Y et B/Z hors tension, A/B≈110Ω, alimentation USB ST-LINK. Aucun recâblage demandé.
[Fiche MIKROE](https://www.mikroe.com/rs485-isolator-2-click) consultée ; préparation005 et schéma v102 distinguent la documentation de la révision réelle.
[Microsoft SerialPort.Write](https://learn.microsoft.com/en-us/dotnet/api/system.io.ports.serialport.write?view=netframework-4.8.1) et [OpenOCD](https://openocd.org/doc/html/General-Commands.html) consultés pour préparer les opérations.

L'absence d'EnableVddIO2 explicite et de pilotage DE//RE est un fait de lecture du code, pas une mesure du banc. Ne pas activer des GPIO ou IO2SV avec une alimentation supposée. Contrôles minimaux décrits : alimentations compatibles, RXD, DE bas /RE bas à U1, sans court-circuiter l'isolation. Si défaut constaté : traiter sous mission adaptée, sans modifier silencieusement l'architecture.

Autorisation/intervention attendue après contrôle ChatGPT : mission physique distincte pour identification/lecture et éventuel flash conforme à la chaîne qualifiée, prérequis électriques et boot vérifiés ; exécution hors ligne HP préalable puis une seule émission. La voie GDB/OpenOCD WSL proposée nécessite qualification/autorisation ponctuelle ; la détection SWD ne la qualifie pas. Le présent rapport ne sollicite aucune opération matérielle en006.

## Publication

Permissions exercées : commit et push normal sur main, **aucun flash/debug cible/émission série/geste matériel**.
Avant publication : main et destination explicite vérifiés, origin/main `c8e2b5c...` sans divergence ; index/périmètre/secrets et validations contrôlés.
Premier push normal réussi `git push origin HEAD:refs/heads/main`, `c8e2b5c → 41ed8c02fee26989d2e2a67cd52dff53b4afe795` (commit du rapport provisoire). Puis fetch : origin/main = HEAD. Relecture des **8 livrables complets, 158474 octets**, via `git show <SHA distant>:<chemin>`, décodage intégral et comparaison octet pour octet aux fichiers locaux : aucune différence. ID de mission et result_sha vérifiés. Manifest des empreintes hors Git : `/tmp/tr2-mission006-remote-reread.json`.

Arbre propre après première publication ; aucune divergence, stashes conservés. Cette mise à jour DONE est publiée dans un commit séparé, puis contrôlée à nouveau ; son SHA est fourni dans la réponse finale sans référence autoréférentielle. Le contrôle indépendant ChatGPT reste distinct de cette relecture Codex et de la qualification physique.
