---
mission_id: TR2-20261012-M015-L-ST-BENCH-PHYSICAL
status: BLOCKED
base_ref: main
base_sha: fccb40cf799c753b9897e74ae6cc6e737a07383c
initial_head: 8a8596fddd66efbd437578902fc4112bbbfa62b6
result_sha: e222dc8ddb7cfd0829c655533ac8e646220721f2
created_at_utc: 2026-10-12T20:09:14.277175+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-L — Essai WHO unique refusé, seconde carte restaurée

**BLOCKED technique après un échec physique réel : WHO_AM_I reçu `00`, attendu
`7B`.** Préflight confirmé, sauvegardes et flash/readback réussis. Arrêt au premier
écart conformément à la mission : polling, FIFO strict et STATUS10 **non exécutés**.
Aucun nouvel essai pour obtenir un succès. La cause n'est pas établie.

## Préflight et acquis réutilisés

HEAD main réel `8a8596fddd66efbd437578902fc4112bbbfa62b6`, seul contrat changé depuisfccb40c. AGENTS/protocole,
journal, rapport K et pinout/protocole/audit relus ; archives K exactes depuis
fccb40c. Worktree isolé `MSM_vibe-M015-L` / `codex/m015-l`, original main82169af
etquatorze autres worktrees/HEAD/stashes préservés. Pas de merge/rebase/resetGit.

Réponse initiale utilisateur «c'est tout bon normalement» insuffisante pour
GND/tensions : seuls artefacts et inventaireSTLINK lus pendant l'attente.
Réponse ciblée suivante : **«3,3V sur les deux VDD, ground sur la broche GND,
et cablage correct MKIGIBV2 conformément à ta demande»**. Câblage demandé
PC9/CN8-4→JP2-21, PC10/CN8-6→JP2-22, PC11/CN8-8→JP2-24, PC12/CN8-10→JP2-23,
VDDIO/VDDJP1-1/2 et GNDmarqué confirmés par l'utilisateur. NuméroGND non déclaré ;
conformité/orientation/absence d'autres modules rapportées, pas inspection
physique indépendante ou mesures sur chaque broche par Codex.

Inventaire : une seule sonde **0032001F3235510C37333439**, NUCLEO-U575ZI-Q,
V3J15M7. GDB UID réel **001B00463842501820333157**, CLI ID482/RevU/3,28V/CortexM33/
Flash2MiB. Identités TR2principal strictement refusées et jamais sélectionnées.

Quatre BIN/ELF/MAP K rehashés exacts, sources du projet identiques. Les **41
scénarios C, 5 tests Python et 4 cross-builds K** sont des acquis conservés,
**non relancés**. Aucun codefirmware modifié ; pas de validationlogicielle nouvelle
nécessaire pour cette mission physique/documentaire. Helper local préparé et
corrigé, pas assimilé aux tests firmware ou à une preuve physique.

## Étape A réellement observée

BackupFlash2MiB **fraîche doublelecture byte-identique** avant toute programmation,
SHA **fcea004eb0c91af44f8e36ca61206b5f6bf25fd4c5db4dfdab42d1a19dcea9d9** ; RAM768KiB
conservée. WHO ST_STRICT source **e222dc8ddb7cfd0829c655533ac8e646220721f2**,
BIN31632 SHA **375fd90243d34eedb9948aff2ba43daabf24c0d0f0a326be2bbee49c1d8f9c21**,
ELF selonmanifestK. Flash NORMAL/SN explicite/08000000, secteurs0..3 uniquement,
verifyCLI et readbackGDB exacts. ResetGDB aprèsdernierCLI suivant procédureM013-D.
Bootready1/UIDexact/core16MHz/faults0, puis **un seul trigger4D31354B** et detach.

| Observation | Valeur réelle |
|---|---|
| Résultat | stage6 / BENCH_WHO2 / cleanup0 |
| WHO attendu/reçu | `7B` / **`00`** |
| Transaction | reg0F / longueur1 / lecture / traceunique |
| HAL et compteurs | HAL_OK0 / error0 / Rxremaining0 / Txremaining0 / valid1 |
| Payload reçu | **`00`**, un octet réel |
| Écritures capteur / XYZ | **0 / 0** |
| GPIO en fin | IDR=ODR00000200 : CS PC9haut, MISO PC11digitalbas |

Commande `8F` reconstructible depuiscode/callback ; RX pendantcommande noncapturé,
pas enregistrementwire de `8F 00`. HAL_OK atteste le parcours logiciel, pas
identité d'un capteur répondant. GPIO lu côté MCU ne mesure pas la tension capteur
ni les fronts. SPI3 mode0/div8/noyau16MHz→2MHz calculés, pas fréquencewire mesurée.
Globals controls/XYZ/FIFO/statuts sont BSSnonrenseignés, **pas des mesures**.
Aucun CSVXYZ fictif, aucune acquisition valide annoncée.

PremièrecaptureGDB arrêtée sur `GPIOC` (macro nonexportée), avantdumps. Capture
complétée uniquement par lecturemémoire, adresseNS42020800 vérifiée CMSIS, sans
reset/flash/trigger supplémentaire ni transactioncapteur. Traces/payload etRAM
récupérés avant restauration ; erreur et correction conservées dans les preuves.

## Restauration et état final matériel

Flash complète aprèsWHO lue ; tous les octets après08008000 sont identiques au
backup. **Seuls secteurs0..3 restaurés** depuisbackupfrais32KiB, procédurestandard
verify/reset. Relecture **Flash2MiB intégralement byte-identique** à sauvegardefraîche,
SHA **fcea004eb0c91af44f8e36ca61206b5f6bf25fd4c5db4dfdab42d1a19dcea9d9**.
ResetGDB aprèsdernierCLI puis lecture sansreset : PC08000c80, UIDexact,
VTOR08000000, CFSR/HFSR0, detach/serveursexit0. Cible laissée en exécution.
Firmware initial nonidentifié fonctionnellement ; restaurationdesbytes ne vaut
pas sa qualification applicative. AucunwritecapteurWHO, pasrestaurationcapteur
supposée nécessaire. Aucun masserase/OptionBytes/RDP/HOTPLUG/média/COM7/RS485.

**TR2 principal jamais connecté/flashé/debuggé/reset**, M014-H et FRAM/SD
préservés ; étatJ historique nonréobservé, pas requalificationM014 ou repriseHP.

## Preuves, limites et prochaines actions

[Preuves publiques](preuves/TR2-20261012-M015-L/README.md) :55fichiers d'observations,
commandes/logsCLI/GDB, snapshots décodés, payloadhex/trace, manifestations
backup/source/restauration ethashesbinaires. Originaux Flash/RAM/BIN/scripts
conservés **/home/lolo/dev/msm/reports/TR2-mission015-L-20261012T195811Z**.
Logs publicsUTF8/LF normalisés ; manifest reliehashespubliés/originaux.
L'inventaireinitial esttranscrit, passtdoutbinaire ; CLI/GDB ultérieurs confirment
SN/UID/MCU. La confirmationcâblage est une observationutilisateur.

Le banc isolé **n'a pas franchi WHO**. Les gettersST deK ne sontpasatteints,
aucune comparaisonde flux avec I/J possible. I256figé valide etJ4096 refusé
postburst restent acquis surleTR2 avecautresparamètres. Pas attributionprématurée
à module/capteur/SPI3/HAL ; un octet00 peut avoirplusieurscauses, non discriminées.

Prochaine action ciblée, **avant toute reprise d'essai** : vérifier hors tension
MISO JP2-24/SDO→PC11/CN8-8, orientationpin1 nappe auxdeuxextrémités, cheminGND
et VDD/VDDIO jusqu'àla carteMKI208 ; confronter labels/révision au schéma réel.
Pas demande d'oscilloscope absent ni substitution automatique. Le premier refus
consomme l'essai A de cettecampagne : nepasretenter/mode3/fallback sans reprise
explicitement cadrée. [Bilan/chaîne et reprise](../QUALIFICATION_M015_L_BANC_ST.md).
Journal/roadmap actualisés pour échecWHO, arrêtB/C etrestaurationsecondecible.

## Publication

Permissions commit/pushmain, flash/debug exercées danslecontrat etchaîne
explicitement identifiée ; restrictionssurTR2principal respectées. Rapport
BLOCKED porte sur résultatmatériel, pas surseuleincertitudepréflight de câblage.
Résultats publiés au commit **f36425707e97e233f079a9d37381567d92ea761b**, `M015-L: record single WHO refusal and exact second-board firmware restoration`.
Push normal sur main réussi, fetch puis **62 livrables relus intégralement et
byte-identiques** ; code TR2/projetST inchangé, archivesK exactes, mission active
inchangée, quatorze autres worktrees/HEAD propres et deux stashes préservés.
Preuve `publication-readback.json`. Le worktree L était propre après ce push.
Un log public a ensuite été normalisé pour retirer une ligne vide finale,
avec mise à jour du hash publié ; l'original reste conservé, aucun résultat
modifié. Le présent accusé documentaire sera publié et relu à son tour.
SHA final distant et état Git dans la réponse finale. **BLOCKED technique
maintenu** : aucune qualification WHO/polling/FIFO acquise. Revue indépendante
ChatGPT à effectuer.
