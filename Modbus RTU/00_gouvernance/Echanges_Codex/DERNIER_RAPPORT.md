---
mission_id: TR2-20261012-M015-K-ST-OFFICIAL-REFERENCE
status: READY_FOR_PHYSICAL
base_ref: main
base_sha: 0b76875c1a0257362280780f835991395b10fcbe
initial_head: b8dad142d85f2a4bc0f0a72ba6b17c12ca47e65f
result_sha: null
created_at_utc: 2026-10-12T19:44:44.737484+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-K — Préparation officielle ST terminée hors matériel

**READY_FOR_PHYSICAL** : projet autonome compilé et testé, publication en cours.
Aucun flash, debug, reset, accès carte ou essai physique exécuté. L'utilisateur
monte le second banc ; câblage, révisions, point GND et SN/UID restent à confirmer.
M014-H sur la première NUCLEO est préservé sans nouvelle observation matérielle.

## Livrables

Projet [`experiments/iis3dwb_st_reference`](../../../experiments/iis3dwb_st_reference/README.md), indépendant du core/harness TR2 :
CMake, HAL/CMSIS CubeU5 v1.9.0 externe, linker/toolchain existants repris comme
infrastructure, pilote officiel et deux exemples originaux byte-identiques,
portage borné WHO/polling/FIFO, captures RAM et scripts hors cible.
[Pinout proposé](../../../experiments/iis3dwb_st_reference/PINOUT.md),
[audit ligne à ligne](../../../experiments/iis3dwb_st_reference/AUDIT.md) et
[protocole progressif](../../../experiments/iis3dwb_st_reference/PHYSICAL_PROTOCOL.md).
Aucun mapping V1, média, service Modbus ou configuration permanente modifié.

Sources ST figées : exemples `1f848c38e84b4144d55c66fb5411e78bdb9f96f1`,
driver `f83e74fb9945fc4dde4e058e7fcd2bdbded357b5`, BSD-3-Clause, URLs/SHA
par fichier dans `vendor/SOURCES.json`. Gitlink superprojet d995549f indisponible
sur GitHub («not our ref») : driver figé séparément, écart explicite.

Polling conserve WHO/reset/BDU/ODR/FS2g/LP_ODR_DIV_100 et température.
FIFO conserve FS8g/watermark256/batching26k7/STREAM6/ODR/DEC8/timestamp dans
l'ordre ST. Pas de mélange avec les valeurs I/J ±4g/FIFOfigée ou watermark128.
Callbacks ST commande TX puis RX sous CS bas, désormais erreurs HAL propagées,
20ms par appel, compteurs contrôlés et CS relevé en sortie. Horloge MSI16MHz,
SPI3 PCLK3/div8/mode0 : 2MHz calculés, pas mesurés ; PC9..12/CN8 sourcés.
Le boot attend un trigger unique ; WHO seul n'écrit pas le capteur. Polling
64XYZ, FIFO512 entrées **mixtes** par bursts128, RAM bornée/stop500ms/reset100ms.
Sortie XL OFF/bypass/timestamp OFF, résultat cleanup distinct, pas de restauration
présumée d'une configuration antérieure sur ce banc seul. Zéro FRAM/SD/UART.

Deux écarts ST reproduits par mocks avec le vrai driver inchangé : statutFIFO
masqué03F (niveau256 devient0) ; getter de batching compare0x10 au champ4bits
alors que setter écrit0A. ST_STRICT conserve le niveau API et refuse à watermark
si incohérent. Profil STATUS10 explicitement séparé lit un second statut brut
et utilise03FF ; les deux observations sont conservées. Garde de batching brut09=0A,
getter conservé en télémétrie. Aucun patch vendeur/vote/correction d'octet.
Ces erreurs de getters **n'expliquent pas** le refus de configuration J.

## Validations effectives

Commande : `python3 -B experiments/iis3dwb_st_reference/tools/validate.py --output /home/lolo/dev/msm/reports/TR2-mission015-K-20261012T192739Z/validation-final
--cube /home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`.
Logs publics normalisés UTF8/LF et arguments dans [preuves/TR2-20261012-M015-K](preuves/TR2-20261012-M015-K/commands.json).

- CTest **2/2** : **41 scénarios C** (vrai driver ST sur mock : WHO readonly,
  polling/FIFO, défauts getter, transport injecté28positions, timeout/reset,
  stop/overflow/tag/config/IF_INC) et **5 tests Python** (décodeur raw↔CSV,
  bornes, refus identité TR2/câblage absent, plan sans exécution).
- **Quatre cross-builds réussis**, GNU ARM14.2.1/CMake/Ninja/Debug ;
  text31612/data20/bss113300, BIN31632, linker Flash2MiB/RAM768KiB,
  minheap512/minstack2048. Pas mesure de profondeur de pile réelle.
  Quatre warnings Newlib usuels `_close/_lseek/_read/_write`, aucune erreur.
- SHA de chaque source ST vérifié, symboles TR2/FRAM/Modbus/SDMMC/UART init
  absents des ELF contrôlés. Erreurs initiales de préparation corrigées et
  expliquées dans `diagnostic-notes.md`, sans affaiblir les tests attendus.

| Image | Octets BIN | SHA-256 |
|---|---:|---|
| who-ST_STRICT | 31632 | `375fd90243d34eedb9948aff2ba43daabf24c0d0f0a326be2bbee49c1d8f9c21` |
| polling-ST_STRICT | 31632 | `0eea150b47a56d00e0ce9eac53d48fe5c42188e7797b422008f8d4038d162fcb` |
| fifo-ST_STRICT | 31632 | `7ae06204ce280c0907799b2b5631cb2c7045583d7ba77f36eab7e1623b778898` |
| fifo-STATUS10 | 31632 | `033f0c425203c5e874aebeff47202d390d2435d0df6aa1da0177a44c8ca3c1be` |

Validation limitée au projet autonome nouveau : sources TR2/core/interfaces
inchangés ; pas de `tr2_validate.sh` ou suites I/J/M014/.NET rejouées sur code
inchangé. Un dernier build était justifié par l'ajout WHO et la fixation explicite
horloge SPI ; seul un commentaire de licence a ensuite changé, sans effet binaire.
Originaux des logs conservés localement. Sources ST CRLF préservées avec attribut local cr-at-eol ; logs publics sans espaces terminaux. Snapshots sources/hashes, logs numérotés de comparaison et manifests publiés.
ELF/BIN/MAP et premières sorties conservés localement dans **/home/lolo/dev/msm/reports/TR2-mission015-K-20261012T192739Z**.

## Brochage, limites et prochaine étape

Documents constructeur ST consultés via web : MB1549C02, DS13737 AF6,
DB4012/schéma STEVAL et DS12569. Tableau CN8↔MKIGIBV2 sourcé ; GND côté breakout
reste un **point à identifier**, pas de numéro DIL inventé à partir du texte.
Les PDF binaires locaux n'ont pas été récupérés (timeouts) ; aucune empreinte
PDF ni inspection physique/graphique complète prétendue. Référence effective,
orientation, continuités, tensions et absence de charges restent à vérifier sur
le second banc. Si autre breakout/révision : revoir la table avant mise sous tension.

Les tests mock n'établissent pas la fiabilité HAL/SPI physique, CS électrique,
WHO réel, variations XYZ, débit/continuité FIFO, parité/pertes ou timestamps réels.
Le décodeur conserve `accepted=false` ; aucune donnée simulée publiée comme
mesure. Pas de qualification4096..32768 V1, FFT, stockage, calibration ou industrie.
ST_STRICT pourra refuser le niveau attendu ; STATUS10 est une adaptation déclarée,
pas une prétendue démo ST inchangée. La première anomalie matérielle imposera l'arrêt.

Après confirmation du montage et identité distincte seconde carte : qualification
chaîne, sauvegarde fraîche de sa Flash, flash/verify/readback, WHO seul puis
polling puis FIFO selon protocole et bilan de chaque étape. Jamais employer SN/UID
historique TR2. Le planificateur imprime seulement un plan ; aucun outil matériel
n'a été exécuté aujourd'hui. Toute restauration future concerne le backup réel
de la seconde carte ; M014-H sur la première reste intouché.

## Git et gouvernance

HEAD initial distant réel `b8dad142d85f2a4bc0f0a72ba6b17c12ca47e65f`, original main82169af propre et divergent
préservé ; worktree isolé `MSM_vibe-M015-K`, branche `codex/m015-k`, destination
explicitement `main`. Archives J mission/rapport exactes depuis0b76875, preuves
I/J intactes, treize autres worktrees et deux stashes préservés. Permissions
commit/push exercées ; permissions flash/debug déclarées mais non exercées,
restriction matérielle du contrat respectée. Aucun merge/rebase/reset/force push.
Journal actualisé : préparation ST, validations, écarts driver et confirmations
physiques attendues ; J BLOCKED/I256 acquis/M015 IN PROGRESS conservés.
Relecture distante et SHA évalué seront consignés après le premier push.
