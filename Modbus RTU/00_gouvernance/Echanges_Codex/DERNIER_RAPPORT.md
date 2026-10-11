---
mission_id: TR2-20261011-RTU-TX-012
status: DONE
base_ref: main
base_sha: null
initial_head: 30275fe6fb6df7fdc05bb87166407080358d67a5
mission_sha: 548ad3fc8dc45221308878b52256f94daac36cf4
result_sha: 9c62c54c50ff92f5b7f53a6f938b5110c7612085
created_at_utc: 2026-10-11T13:27:41.688521+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Mission012 — chemin de réponse testé, activation physique différée

**IMPLEMENTATION VALIDÉE / HOST TESTS 107/107 / STM32 BUILD RÉUSSI (OFF et ON) / FLASH NON EFFECTUÉ / TX PHYSIQUE BLOQUÉ.** DONE pour cadrage/intégration logicielle et préparation après publication/relecture distante. Le travail de cadrage/intégration logicielle prévu au contrat est achevé ; arrêt matériel appliqué conformément aux clauses4/5. Aucun TX physique ni nouveau RX déclaré validé.

## Références et portée

HEAD initial main propre `30275fe6fb6df7fdc05bb87166407080358d67a5`, fetch/pull fast-forward vers contrat `548ad3fc8dc45221308878b52256f94daac36cf4`. AGENTS.md, protocole V2-B3, rapport011, journal intégral, charte V1 et références B0, freezes P12F/G/H, architecture, sources et tests concernés inspectés. Mapping, types sans float, interfaces portables et sémantique transactionnelle inchangés. Les champs base_sha/date manquent au contrat : base_sha null, références observées consignées sans invention ; permissions et objectif explicites.

Pendant le travail, origin/main a avancé à `c035a7fc226852de9a5041342884f2cca13b4722` par ajout de la seule roadmap. Mission et sources inchangées ; absence de collision vérifiée, dix fichiers locaux hashés avant/après pull fast-forward : préservés à l'identique, sans merge/rebase/reset/stash supplémentaire. Roadmap laissée inchangée, rôle M012 respecté. Deux stashes005/002 conservés.

Commit code **`9c62c54c50ff92f5b7f53a6f938b5110c7612085`**, message **`STM32: gate RS485 response TX with DE, TC recovery and watchdog`**. Archives011 mission/rapport copiées exactement depuis HEAD initial ; preuves physiques011 conservées et pas d'essai RX répété.

## Audit et modifications

[Audit et prochain jalon](../INTEGRATION_RTU_TX_012.md) : la chaîne `main → ModbusSystemServer → ModbusRtuServerRuntime → PDU V1 → ADU → SerialTransport` existe déjà. Le manque traité est le pilotage half-duplex STM32. Aucune réécriture du serveur, aucune adresse/B0 ou dépendance de production inventée.

- `platform/stm32/stm32_serial_transport.{c,h}` : gate TX OFF par défaut ; réponse seulement après T3.5 consommé, queue vide, timer idle et aucun RXNE/BUSY, TX ou défaut. Vérification/armement sous section critique PRIMASK restaurée. Abort RX, PG4 DE haut, watchdog TIM6 50ms, Transmit_IT ; fin **TC** entraîne DE bas puis réarmement RX et garde1750µs. TXE/buffer vide ne relâche pas DE. PG5 jamais piloté.
- Échec de lancement/abort RX, erreur TX, timeout ou réarmement RX défaillant : DE bas avant abort TX, erreur visible et défaut latched jusqu'à nouvelle initialisation, aucun retry automatique. Overflow empêche une réponse sur trame incohérente ; activité plus récente invalide le silence. Callback tardif/écho RX supprimé pendant TX. Erreur RX ordinaire réarme proprement HAL ; réarmement par octet en échec déclenche fail-safe visible.
- CMake STM32 : option `TR2_STM32_RS485_TX_QUALIFIED=OFF`. CMake hôte : deux cibles test du vrai source STM32, macro0 et1, avec HAL simulée isolée. `tests/platform/test_stm32_rtu_tx.c` relie le **vrai serveur applicatif ModbusSystemServer**, son RTU/PDU/codec V1, au **vrai transport STM32** ; fixtures runtime/B0/unit17 seulement dans le test, jamais provisioning carte.
- `INTEGRATION_RTU_TX_012.md`, journal et archives011 : audit, preuves, risques et étapes restantes. Aucun changement freezes, registres B0..B7, services, persistance ou E4.

Le hook faible `stm32_production_binding_acquire` reste NOT_AVAILABLE : carte en diagnostic RX, sans boot artificiel du runtime. Géométrie FRAM, récupération transactionnelle valide (EMPTY non formaté), CampaignDataStore réel, adresse et identité/configuration restent requis. Gate ON ne les fournit pas. Le raccordement logiciel de réponse est démontré sur hôte, pas une activation fictive de production sur STM32.

## Tests et compilations effectifs

Validation complète retenue selon AGENTS.md : changement du comportement transport partagé avec le serveur, risque de régression plausible ; pas de simple cross-only. Commande finale :

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh
```

Retour0, **107/107 tests hôte** et cross-build ARM OFF réussis. Après les dernières extensions des tests uniquement, rebuild des deux cibles et tests ciblés2/2, puis CTest complet final **107/107** réussi. Pas de test affaibli/désactivé. Pas de validation .NET, aucune source PC modifiée.

Nouveaux scénarios : réponse FC03 valide et CRC, exception FC06, CRC erroné, adresse étrangère, broadcast FC03/FC16 sans réponse, silence T1.5/T3.5 et interruption, aucune émission initiale, TX busy, activité plus récente et RXNE/BUSY, overflow, ordre DE→HAL TX→TC→DE bas→RX, garde intertrame, watchdog50ms sans dépendance au polling, erreurs TX/RX/abort et reprise RX en échec, défaut latched, écho, readiness applicative perdue, PRIMASK préservé, gate OFF. Tests de mapping/production existants également passés.

Build ARM ON séparé sous `/home/lolo/dev/msm/reports/TR2-mission012-20261011T132002Z/build-tx-enabled`, configuré avec toolchain vérifiée `platform/stm32/cmake/arm-none-eabi-gcc.cmake`, SDK CubeU5v1.9.0 et `-DTR2_STM32_RS485_TX_QUALIFIED=ON` : build initial et rebuild final retour0. Désassemblage du candidat confirme branche stm32_transmit vers HAL_GPIO_WritePin et HAL_UART_Transmit_IT. Les mocks vérifient le contrat du code plateforme ; la HAL interne et les délais électriques ne sont pas simulés comme une preuve matérielle.

HAL officielle auditée : UART_IRQHandler traite TC/TCIE via UART_EndTransmit_IT et TxCpltCallback ; TXE reste distinct. AbortReceive/Transmit sont opérations bornées sans DMA applicatif. Profil115200/8E1, buffer256 : temps théorique maximal~24,45ms, watchdog50ms. Timer1MHz, IRQUART/TIM6 priorité5, queue64 (63 utilisables), T1.5=750µs/T3.5=1750µs conservés. Quatre avertissements newlib existants `_close/_lseek/_read/_write`, aucune erreur ni masquage.

## Artefacts évalués, non flashés

| Build | BIN octets / SHA256 | ELF SHA256 | text/data/bss |
|---|---|---|---|
| OFF, défaut | 157784 / `8f9e0cea42cbf02b2b312fb0f1cfc6f07df2af20b054a801c6f3379314949ff1` | `9e6d0c4986fbaf636fcdf8f6254078980be16be4fb4ed882e72e3bfafd1cd731` | 157660 / 124 / 129072 |
| ON, candidat seulement | 159152 / `ce3c8b2e850da332cf43858e9b633abb217cd700f8d09218a44b27049f08ae53` | `2f534de2ad8fc4af5415b453f7eda1c2b5645b59a9d9855b2cc0dfaccb8b02cd` | 159028 / 124 / 129072 |

BIN/ELF/MAP008 préservés et empreinte BIN contrôlée **avant** reconstruire les builds locaux : `/home/lolo/dev/msm/reports/TR2-mission012-20261011T132002Z/preserved-BIN008`. Carte laissée sous BIN008, **aucun flash/debug/USB/reset/émission en012**. Ne pas utiliser nouvel ELF012 sur cette carte. Copie des artefacts OFF, builds ON, logs finaux, désassemblages et `artifacts.json` (tailles/SHA256) hors Git sous `/home/lolo/dev/msm/reports/TR2-mission012-20261011T132002Z`. Logs `/tmp/tr2-mission012-validation-final.log`, `...-ctest-final.log`, `...-targeted-final.log` copiés dans ce dossier. Aucun binaire public nouveau.

## Sûreté et jalon physique restant

Acquis : PG4 DE câblé, PG5 et /RE sans fil, pont J2 soudé rapporté ; schéma v102 J2=DE–/RE, J3=/RE–RST ; RX011 exact et PG4 entrée digitale basse. Ces observations sont conservées, pas redemandées. Faits absents pour TX : révision/continuité commune et absence commande RST concurrente, VIO Click réellement compatible3V3, /RE conforme lors de DE haut, direction/turnaround dynamiques. VIO=VCC dans schéma, rail SB28 ne mesure pas VIO Click.

[ADM2867E RevD table2](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf) consultée : enable/disable DE max40ns sous conditions constructeur. La séquence prévoit des opérations HAL/TIM avant démarrage UART ; aucun délai physique mesuré revendiqué. Vérification RXNE/BUSY/silence limite réponse obsolète, sans garantir contention/multimaître ou timing sur bus. La garde n'est pas une qualification de réception d'un maître violant le silence.

**Limites fail-safe explicites** : DE bas au boot après initialiseur008, pas de garantie matérielle pendant reset avant prise en charge GPIO. Watchdog dépend de TIM6/IRQ fonctionnels ; ne garantit pas libération sur panne CPU/IRQ masquées. Pas de nouveau pont/pull-down physique inventé. Une garantie permanente requiert décision matérielle et preuve spécifique.

Conformément aux clauses4/5, pas de flash ni essai TX quand ces conditions restent incertaines. Étape préparée : rassembler seulement les faits TX manquants, fournir binding carte réel, puis build ON identifié/validé, sauvegarde2Mio/SN/hash, flash standard vérifié/relu et ELF correspondant sous contrat physique, un essai requête/réponse contrôlé avec instrumentation direction/TC/turnaround. COM7 HP inaccessible ici ; aucune nouvelle émission ou répétition RX demandée. Le contrat permet cette clôture de préparation avec jalon physique différé ; pas d'autorisation supplémentaire sollicitée pour du logiciel déjà autorisé.

Journal actualisé : tranche transport, tests/builds, garde OFF et runtime indisponible, BIN008 conservé, prérequis TX/reset. À ChatGPT : revue indépendante du code et des preuves, puis cadrage M013 suivant roadmap ; J1/TX et E4 toujours à qualifier.

## Publication

Diff/index, whitespace, chemins et secrets inspectés, aucun fichier hors périmètre. Permissions effectivement exercées : développement, documentation, tests/compilations, commit et push normaux main ; flash/debug permis mais non exercés. Aucune opération destructive ou physique. Push normal main réussi, HEAD=origin/main=`d9b5c8d183a998784cc73bdb6619952875485f90` après fetch. Relecture intégrale des onze livrables modifiés et du contrat012 inchangé depuis cette référence distante ; égalité byte-à-byte locale, identifiant et result_sha contrôlés. Manifeste hors Git `remote-reread.json` dans dossier de preuves012. Arbre propre, deux stashes conservés. Cette mise à jour DONE sera publiée puis relue ; SHA final dans la réponse Codex. DONE concerne la tranche logicielle/préparation explicitement prévue, pas TX physique ni J1.
