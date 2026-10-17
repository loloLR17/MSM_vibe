---
mission_id: TR2-20261017-R5AFIX-B3-WIRE-ORDER
status: IN_PROGRESS
base_ref: main
base_sha: e100770474cf4d43a73f2af2e6e02be91e2cd75c
initial_head: 5103ea4e87c169c1b154d0df8cdcc50511619a24
result_sha: 770aef24831d796609c859549ee2e2fb14e21af3
created_at_utc: 2026-10-17T11:44:51.663493+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R5-AFIX — correction démontrée et matériel restauré

**Critères techniques A–E satisfaits dans le périmètre ciblé.** Statut IN_PROGRESS uniquement jusqu'à publication et relecture distante de ce bilan ; aucun blocage technique actif. L'inversion3002/3003 est corrigée dans le firmware, démontrée par de nouveaux ADU physiques et acceptée par le décodeur PC inchangé. R5-A historique reste BLOCKED dans son archive ; aucun GO R5-B/R6 ou industriel implicite.

## Correction et preuves indépendantes

Code firmware **`29a6f4ecf83c97499055b58fc73d631763cf5813`**, `fix(r5afix): project LIVE B3 alarms and severity at normative wire offsets`. Deux destinations modifiées dans `live_metrology_project_b3` :3002alarmes,3003sévérité. Les expressions, masques, latch, dépassements et règles de qualité restent identiques. Norme, R2/B5, runtime FIFO et décodeur PC de production inchangés. [Note technique](../../05_Firmware/LIVE_R5AFIX.md).

Nouvel oracle indépendant : neuf états avec attentes littérales de mots et d'octets ADU, échec sur l'ancien projecteur puis réussite après correction. Boot/attente, normalUTC, frais sansUTC, dépassement0xF0/3, alarme+latch0x1FF/4, HELD0x1FF/0, latchseul0x100/2 et erreur0x100/0. Une ancienne assertion qui contrôlait la sévérité au mauvais offset est corrigée vers3003 ; son contrôle reste exigé. Revue de table ciblée des offsets0..47 et des chemins de projection/transfert : aucune autre inversion adjacente démontrée, pas de correction opportuniste.

La fixture C exportée est explicitement une simulation. L'oracleC# indépendant vérifie le mapping, la qualité et SQLite. Les2068anciensB3 R4-U/V restent refusés ; aucune tolérance de sévérité240 et aucune permutation côtéPC. Le domaine historique sévérité0..5 n'est pas modifié ; le LIVE conserve sa formule0..4 sans critique5 automatique.

## Résultat physique unique

Préflight frais11:36:20UTC : cibleTR2-001/SN003500463235510B37333439/UID004A003D/38425018/20333157, FlashM0162Mio c5bc…f5b26 et FRAM70e…73587 entières exactes, WHO7B/capteurarrêté. COM4FTDI0403:6001/BG04KSSW retrouvéT1000 et aucun maître natif concurrent répertorié ; port exclusif1152008E1. Aucune nouvelle manipulation physique nécessaire. Sauvegardes et procédure qualifiées, pas de HOTPLUG.

La lecture readonly initiale0/0 ne distinguant pas les offsets, un seul START/STOP R2 autorisé a été exécuté après publication/relecture23c6bd3. Gardes exactes journal62/root75/epoch5/run9/config3gen3 avant admission ; **START5/10/run10 puisSTOP5/11**, terminaux4/0, sans retry/collision/refus/RENEW supplémentaire. Aucune autorité B3 inventée. GDB aux checkpoints avant START et après STOP, sans pause pendant acquisition.

| Mesure réelle | Résultat |
|---|---|
| ADU COM4 |1095 réponses CRCexactes ;563B3,462V/F,51HELD |
| Première V/F |3002=0x00F0,3003=3 ; statut4/flags1215/séquence1, UTC0 indisponible |
| HELD après STOP |3002=0x00F0,3003=0 ; statut1/flags5819/séquence34/FRESH0 |
| FIFO |1784856XYZ/223107TS, niveau max70, erreurs0 |
| Work/service/gap |809/2116/2344µs sous1000/2500/5000 inchangés |
| UART |0drop/0erreur/0TXfailsafe |
| Oracle4096 brut indépendant |RMS954/953/10/27mg, peaks996/996/34/73mg exacts MCU |
| Terminal maître |START39,893016s ;STOP39,967197s |

Ces temps ne constituent pas une borne d'arrêt industriel. Pas de calibration vibratoire, endurance ou WCET isolé. Les cas alarmeactive/latch/erreur sont démontrés par oracles hôte, sans provoquer un défaut physique supplémentaire. Les valeurs RMS et leur datation observée sont séparées de l'UTC absente ; aucune métadonnée run/config n'est ajoutée aux registres réservés.

## Validation PC et logicielle

Code de test PC **`770aef24831d796609c859549ee2e2fb14e21af3`**, `test(r5afix): accept corrected physical B3 capture through PC and SQLite`. La source firmware y est identique à celle du flash23c6bd3. Les563B3 réellement capturés sont décodés et persistés563fois dans SQLite, sans quarantaine ; le contexte non exposé reste null et HELD n'est pas courant. Ce replay utilise des headersB4/B6 **simulés et étiquetés** ; B0/B3 authentiques, pas une connexion PC série de production ni une preuve R5-B.

- `bash ./tr2_validate.sh /home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0` : **146tests complets PASS96,02s et cross-build canoniquePASS**. Pertinent car cœur fonctionnel modifié ;145anciens tests conservés, V1/protocole1 et barrièresR2 couverts. BINcanonique85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5 inchangé et non flashé.
- Probe dédié NON-PRODUCTION ARM PASS, recette explicite dérivée R4-U/R4-V, pacing250µs inchangé. BIN147640octets SHA **d50fe6bbdde1b5127ee9cd2c5f3ffcbaae5045453153df71eb6e793a2b13f99b**, ELF **9b10196a43d08ee75fd28df68e7a77e12b66f599d47494c411a7d737aa2da040** ; flash normal verify puis readback image conforme.
- Quatre suites O2/ASan/UBSan PASS8,80s : métrologie, offsetsADU, serveurLIVE et pacing runtime.
- **202tests Service.NET PASS** avant physique, dont anciens négatifsR5-A et fixtureCpositive/SQLite ; après ajout du testphysique, **2tests R5AFIXPASS** (fixture et capture). Les autres539tests.NET R5-A surcode inchangé ne sont pas tous relancés ; pas de prétention à une nouvelle suite.NETcomplète553.
- Échecs conservés : premier buildoracle rejeté parWerror pourindentation, corrigé ; essai suivant échoue réellement sur offsetsavantcorrectif ; lancementdirecttr2_validate refusé carfichier nonexécutable, reprisevia bash réussie. Aucun contrôle supprimé/affaibli ni succès déduit d'un code de retour agrégé. Les logs bruts gardent CRLF et espaces des outils.

## Persistance et restauration vérifiées

FRAM initiale complète262144octets70e273972a83e61283a6e572db3187af002fd97ba8de63f8c709d3e30af73587 ; finale **4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd**, entière exacte au modèle utilisant le vrai store/R2. Journal62→70/root75→83/epoch5/run9→10/config3gen3, STOP5/11COMPLETED/effect45phase3.419408octetsécrits/8commits seulementLIVE ; historique/tail inchangés. Aucun recyclage d'identité, retraite, RENEW ou rollbackFRAM.

**M016 CommissioningReadOnly WAIT restauré11:41:55UTC**,19pagesnormales155648octets. RelectureFlash2097152octets entièrement identique àl'initiale **c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26**. FRAMentière relue après restauration exactement4e7f…84bd. WHO7B/ODRoff/FIFObypass/TSoff lus ensuite, COM4fermé/GDBdétaché. Aucun SD/HP/câblage/OptionBytes/RDP/masserase ni firmwareLIVEproduit laissé sur carte.

## Traçabilité, limites et suite

[Résumé physique](preuves/TR2-20261017-R5AFIX/physical-summary.json), [auditFRAM](preuves/TR2-20261017-R5AFIX/final-fram-audit.json), [restauration](preuves/TR2-20261017-R5AFIX/hardware/restoration-summary.json), [provenance](preuves/TR2-20261017-R5AFIX/source-provenance.json), [manifestes et artefacts](preuves/TR2-20261017-R5AFIX/BUNDLE.json). Les captures brutes, BIN/ELF, snapshots, logs initiaux et finaux et sources de vérification sont conservés avec SHA brut/stocké ; zlib uniquement pour compresser. Répertoire opérationnel `/home/lolo/dev/msm/reports/TR2-r5afix`.

Worktree isolé `MSM_vibe-R5AFIX-b3-wire`, branche `codex/r5afix-b3-wire` issue du main distant vérifié5103ea4. Ancienmain82169af propre,8077fichiers protégés,64autresworktrees, étude86d0f4b etdeuxstashes6251b988…/f3b829e6… préservés. ArchivesR5-A BLOCKED byte-identiques. Permissions commit/push/flash/debug exercées dans le contrat ; aucune norme/gouvernance/detteA04 modifiée. Journal etroadmap actualisés factuellement avec correctif, nouvellepreuve etétatmatériel.

Critères A mapping/B qualité-V1/C PCpositif+négatif/D portée/E préservation démontrés. Publication de ce bilan et relecture distante restent àfaire avantDONE. R5-A n'est pas réécrite rétroactivement ; contrôle indépendantChatGPT requis. Suite sousnouveaucontrat R5-B : composition maîtreCOM4unique→SQLite→Web, vraisB1/B2/B7/capacités/profil complet, puisR6. Les latencesR4-V restent importantes ; aucun GO industriel, tag ou jalon global qualifié ici.
