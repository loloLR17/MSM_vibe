---
mission_id: TR2-20261013-M015-Q-FIFO-REPEATABILITY-WINDOWS
status: BLOCKED
base_ref: main
base_sha: 6ea0468b43aa966a98a4037db5b741a0ec98178f
initial_head: 0e20ade0bba44db1206f4af47ca0c30dfca70bce
result_sha: 76b4a17577171c0009ada7e0c67e387286d282dc
created_at_utc: 2026-10-12T22:41:11Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-Q — répétabilité et 512 XYZ acquis ; 4096 bloqué par overflow FIFO

## Résultats physiques et arrêt

| Capture unique | Mots FIFO | XYZ / timestamps | XYZ distincts | Résultat / cleanup | Conclusion |
|---|---:|---:|---:|---|---|
| Témoin P, reset MCU n°1 |296|263 /33|263|0 /0|VALIDÉ PHYSIQUE BORNÉ|
| Témoin P, reset MCU n°2 |296|263 /33|263|0 /0|VALIDÉ PHYSIQUE BORNÉ|
| Témoin P, reset MCU n°3 |296|263 /33|263|0 /0|VALIDÉ PHYSIQUE BORNÉ|
| Fenêtre512 |576|512 /64|509|0 /0|VALIDÉ PHYSIQUE BORNÉ512|
| Fenêtre4096 |2560|2275 /285|2263|7 /0|REJETÉE, cible4096 non atteinte|

Les cinq bruts sont reconstruits indépendamment depuis les transactions SPI et comparés byte par byte au buffer RAM officiel. Tous les mots reçus sont conservés, y compris ceux de la fenêtre rejetée. Dans chaque capture : parité/counter discontinuities/composantes saturées zéro et huit XYZ entre timestamps successifs. Les trois témoins ont un pas timestamp24ticks ; les fenêtres512/4096 ont des pas24 ou27ticks, conservés sans correction. Ce résultat ne démontre pas l'absence globale de pertes : les compteurs modulo4 ne détectent pas toutes les pertes, et la dernière fenêtre comporte un overflow explicite.

**Premier refus4096 : trace151 (indice zéro), tick5627, FIFO_STATUS raw `00 EA`, niveau512, flags `EA` (FULL, overrun et latch).** Vingt segments de128mots avaient été reçus, soit2275XYZ+285TS ; result7/BENCH_OVERFLOW. Le service s'arrête avant tout segment supplémentaire. Les mots de ce préfixe sont reçus et décodables, mais ne qualifient pas une fenêtre4096 ni une fenêtre sans pertes. Aucun nouveau test physique, réglage ou variante après refus.

Le cleanup est réussi dans tous les cas : CTRL1_XL=0C (XLoff), FIFO_CTRL4=80 (bypass), CTRL10_C=00 (timestamps off). UID et CPU16MHz vérifiés, CFSR/HFSR0, detach et serveurs terminés. La seconde NUCLEO reste sur l'image4096 **inactive après cleanup** ; aucune restauration ni backupFlash2MiB injustifiés. Le TR2 principal/M014-H, FRAM, microSD, partition, Modbus et HP n'ont jamais été accédés.

## Mesures et diagnostic de débit

Les bursts896octets durent environ3,631..3,634ms par DWT, soit246,5..246,8ko/s pour la phase payload. Cette mesure ne couvre pas tout le service. Dans4096, les départs des vingt segments s'étalent sur94ms ; les dix-neuf intervalles transfèrent17024octets, soit **181,1ko/s** pour la cadence de service observée. Les niveaux avant les segments croissent de148 vers507, puis le statut final atteint512/EA. Le drainage complet ne suit donc pas le remplissage sur cette exécution, malgré des appels SPI complets et sans erreur.

Le premier refus se situe114ms après first_tick de la fenêtre. Le champ last_tick firmware reste0 sur échec : une durée dérivée par soustraction modulo n'a aucune valeur ici. La première analyse contenant cette durée invalide est conservée ; l'analyse corrigée met duration_ms=null et calcule le délai jusqu'au statut de refus à partir de sa trace. Fenêtre512 :38ms incluant attente initiale, transferts et contrôles ; niveau max/final252, flags_OR0. Cette durée n'est pas une mesure indépendante de l'ODR.

Pour4096 :164traces/18376octets/328snapshots,48statuts ; overflow du journal0 et aucun refus de capacité/HAL/SPI. Les budgets RAM/journal ne sont pas épuisés. Le coût exact entre parsing, contrôles de configuration, HAL et journalisation n'est pas isolé par ces observations ; aucune cause particulière n'est inventée. La récupération O/P et sa relation à la reconnexion de nappe restent **CAUSE NON ÉTABLIE** : Q qualifie la répétabilité actuelle, pas cette RCA.

## Méthode, code et garde-fous

Le témoin immuable BIN P/O mode3, SHA `ed19310485d51af22b18420ea8af3ff3acba36531b3e9741bc78e91792f282f5`, était déjà présent en Flash. Relecture byte-identique et identité seconde avant chacun des trois resets GDB ; aucun flash du témoin. Trois déclenchements uniques, sans manipulation de nappe/module/alimentation. Reset MCU GDB distinct du reset logiciel capteur exécuté ensuite par la fonction ST ; aucune coupure d'alimentation manuelle ou mesurée.

Le nouveau service `fifo_window_run`, l'option BENCH_WINDOW_XYZ et le port conditionnel restent isolés dans `experiments/iis3dwb_st_reference`. La vraie fonction ST générée initialise le capteur ; à sa première demande FIFO_STATUS, le service remplace seulement la boucle drain/console par un drainage segmenté. Driver vendor, générateur, source ST, transport commande/RX, CPU16MHz, SPI2MHz/mode3, HAL_SPI O2 et réglages capteur inchangés. Le nouveau module de parsing est O2 ; le témoin par défaut reste byte-identique après cross-build.

Cible exacte XYZ et timestamps distincts ; segments<=128mots/896octets, dernier segment limité au nombre de XYZ restant. RAM4736mots (33152octets) pour accumulation, pas de supposition4096mots dans le FIFO capteur. BSS309456/data476, réserves heap512/stack2048 sur RAM786432 ; marge d'environ474000octets. Budget transport1024traces/65536octets/2048snapshots et réservecleanup12/32/24 inchangés. Statut dix bits avant/après segment, refus mask68/niveau>=512, maximum400statuts/500ms, stop, config définie avant/après, parité/counters/tags/timestamps/saturation et snapshots avant fermeture contrôlés.

[Critères établis avant extension](../../../experiments/iis3dwb_st_reference/QUALIFICATION_M015_Q.md). [Datasheet ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), table8 :11 et18 sont réservés ; ils restent dans les bruts, sans valeur de configuration imposée. Première analyse répétabilité rejetée pour comparaison erronée18=E0 contre E2historique : erreur propre à l'évaluateur conservée et corrigée sur les bruts existants. Aucun garde-fou cible refusé ou affaibli ; aucune quatrième capture.

## Validations logicielles réelles

Validation ciblée justifiée par l'expérimentation autonome et l'absence de modification du cœur TR2 ou des interfaces V1 : CTest `bounded_fifo_xyz_window` réussi, dont succès synthétiques512/4096,39positions IO, capacités/timeout/stop, overflow avant/après drain, corruption config/parité/counter/tag/saturation/timestamps/ratio, conservation des bursts rejetés et tick wrap. Quatorze scénarios d'intégration avec vraie fonction ST générée/vrai port réussis : succès et refus IO/overflow/parité/config/capacitéjournal/cleanup pour512 et4096.

Trois builds ARM réussis : contrôle/512/4096 ; contrôle byte-identique P. Warnings newlib usuels conservés. Premier mock échoué parce que le compte attendu incluait le timestamp suivant le dernier XYZ non reçu ; correction des attentes exactes du flux synthétique575/4607mots (63/511TS), sans changement de l'algorithme ni des gardes. Logs d'échec et corrections conservés. Aucun ancien test réussi rejoué sur code inchangé ; pas de nouvelle suite cœur/.NET.

## Artefacts et matériel

Code fenêtres réellement évalué : `76b4a17577171c0009ada7e0c67e387286d282dc`.

| Image | BIN SHA-256 | ELF SHA-256 |
|---|---|---|
| Témoin P utilisé |ed19310485d51af22b18420ea8af3ff3acba36531b3e9741bc78e91792f282f5|9d5f4979d16d8fd07d6228e603ca5ce8b05b4b2a7673bb7415812fe44a930145|
|512|f9dab29cd348efa7d2bc70a1bf3d86a2624f59ac7429f3d3defa82c779874e1a|2d889b8f6aacc8c477fe091f363932330c52a09dd82884094d38f5d30e67fd66|
|4096|d76a6104a2adc19f77e3b9849e32b9d52312dfd2840eb01f76eb4749cf687b09|ac60234c6223a199fc0095477c2599029aaebe131c173b7fa35dfe3b75459a09|

SNSTLINK `0032001F3235510C37333439`, UID `001B00463842501820333157` uniquement. Chaque flash512/4096 : identité, hashes, programmation standard SWD08000000/verify, reset CLI documenté puis relecture GDB exacte et reset GDB final, contrôle ready/faults avant triggerunique. Tous les scripts et codes de retour GDB/serveur0 sont conservés ; retour7 de qualification offline4096 signale le refus réel, pas un succès logiciel masqué. Aucun mass erase/Option Bytes/RDP ni intervention physique.

## Préservation, publication et suite

HEAD initial mission `0e20ade0bba44db1206f4af47ca0c30dfca70bce`, worktreeQ isolé. Dépôt principal conservé au82169af ; tous les worktrees historiques HEAD inchangés/propre et deux stashes conservés. Archives P mission/rapport copiées exactement depuis6ea0468 et vérifiées. Journal actualisé pour répétabilité/512/acquis4096 rejeté/état final ; roadmap M015 IN PROGRESS, continuité4096, V1, FFT/stockage non qualifiés.

Checkpoints publiés et intégralement relus : répétabilité06038823d0e80963d8d6f85a85a774a9fe4ff141 (126fichiers), logiciel76b4a17577171c0009ada7e0c67e387286d282dc (45), physique512 `5b5443cf14d4033bf6cf5c19d87f9fd1b66b00d7` (48). Les journaux CLI normalisés comportaient des blancs finaux ; correction documentaire avec conservation intégrale des originaux en.hex, sans changement d'artefact ou résultat. Rapport final et preuves à publier/relire ; statut **BLOCKED technique4096** maintenu après publication.

[Corpus et matrice](preuves/TR2-20261013-M015-Q/README.md), originaux `/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z`. Suite à contractualiser : établir un budget du **service complet** et mesurer/optimiser son coût hors boucle SPI, ou préparer un transport/service adapté (éventuellement DMA) en conservant tous les contrôles et cleanup. Reprendre ensuite uniquement l'étape4096 sous contrat, sans rejouer les trois témoins ni512 sur code inchangé. Conserver montage fonctionnel ; aucune recherche de faux contact ni emprunt du primaire.
