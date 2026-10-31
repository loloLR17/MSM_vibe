---
mission_id: TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE
status: IN_PROGRESS
base_ref: main
base_sha: f99e85cc3857aa6dd9db60b16c4a37ba1c7550b0
initial_head: 44e0569e4241c54daab30bf9abd8b06de2ce2320
result_sha: null
created_at_utc: 2026-10-31T13:44:08.371872+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Préqualification hors ligne — checkpoint logiciel

Mission récupérée sur origin/main ; travaux isolés dans MSM_vibe-R5-prequal,
anciens worktrees/stashes et analyse PK préservés. HEAD initial vérifié par Git : `44e0569e4241c54daab30bf9abd8b06de2ce2320`.

Lot A en cours : 16 chaînes globales causales (deux hypothèses de cadence,
quatre bandes, total plat, test LP10 non normatif, variantes de borne basse).
Lecteur strict/streaming/qualité/settling R5 réutilisés. **5 tests ciblés PASS** :
oracles analytiques, sinus/multitons/AM/FM/impulsions/offsets/gaps/clips,
indépendance du découpage et Parseval. Tolérances fixées dans METHOD.md.
Le parcours réel des six heures est lancé ; ses résultats ne sont pas encore
proclamés. Agrégats sauvegardés avant contre-calculs pour conserver le travail.

Lot B : audit source établi, archive logique8g/244µg par compte ; le firmware
R3 appelle fifo_live_start(...,fs=2) avec readback initial contraint à 0xAC.
Court historique précédant le long : ctrl1=172 observé. Registre de fin du long
non récupéré après coupure : aucune lecture actuelle ni preuve matérielle
continue inventée. ST DS12569 Rev8 consultée : bruit indépendant du FS et
caractérisé de 100Hz à6,3kHz, pas garantie de bruit à1Hz. Extrêmes des sorties
archivées et budget quantification à terminer.

Suite : terminer le parcours et ses contre-calculs, comparer les mêmes périodes
60/120s, simulation honnête de résolution, copie migrée v4 nécessaire puisque
v1/v2/v3 sont toutes interdites en écriture, figures et note de décision,
puis rapport final/push/relecture. Aucun matériel/STM32/SD/COM/Windows ni achat.
Aucune métrique ISO/Wm/BV ni seuil de confort. Ancienne R3 reste sous réserves.
Preuves locales : /home/lolo/dev/msm/reports/TR2-R5-prequal-20261031/.
