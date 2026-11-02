---
mission_id: TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR
status: IN_PROGRESS
base_ref: main
base_sha: a8817bf293c6e60a93ad824d279ee870b6a0ecf7
initial_head: 79b952dbb949cb8eb2f208e3b2c5994f4ed1279f
result_sha: acb142794570ded5abd15f6521eb5d8b644590e8
created_at_utc: 2026-11-02T08:55:54.965107+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# TR2-001 — D05 après déplacement du module B sur breadboard

**IN_PROGRESS, attente de mesures physiques.** L'utilisateur préfère préserver
le câblage éprouvé de TR2-002. Il confirme avoir déplacé uniquement le module B
sur breadboard, hors tension, en conservant les fils ; lecteur SD et jumpers
inchangés, puis TR2-001 alimenté par ST-LINK. Aucun fil neuf ou provenant de002
n'a donc été testé. Les mesures3,315V de D04 précèdent ce déplacement.

Contrôle sans build, flash, reset ou START : UID001 exact, même BIN D01 de50912B
relu intégralement à l'identique, source `acb142794570ded5abd15f6521eb5d8b644590e8`,
SHA-256 `6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2`.
**WHO_AM_I reçu =0x00, attendu0x7B**. Une transaction SPI conservée : registre0x0F,
longueur1, payload00, HALstatus/error0, RX/TXrestants0. Le succès HAL ne démontre
pas la qualité électrique de la liaison.

Arrêt99/error1/run0 avant réglage FS/calibration, zéro acquisition. FIFO jamais
démarré, writer inactif, CS SD haute, commandes/lectures/écrituresSD0,
pilote non initialisé/gate écriture fermé, GDB détaché. Aucun accès002/FRAM/COM4.
Pas d'appui USER demandé, ni de60s forcé, ni de test ancien répété.

Le défaut observé après déplacement diffère de D04 : l'identification échoue
désormais avant la calibration. Pas de preuve de module ou MCU défectueux.
**Prochaine action utilisateur demandée : mesurer VDD, VDDIO et GND du module par
rapport à GND Nucleo**, sans modifier le montage alimenté ; attendus3,3/3,3/0V.
Aucune nouvelle mesure n'est encore acquise. Le prochain contrôle dépend de ce retour.

[Preuves D05](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/breadboard-B/D05_RESULTAT.md).
HEAD initial de tranche `314ec12960635b814fe6bee3c5535fc59bc66d7c` ; documentation
et preuves seulement, pas de modification firmware. Journal actualisé.
Checkpoint `871ce9c01645d4f3685e01e03ef7c637d08a5205` publié sur main et relu : 18 fichiers identiques,
610 empreintes de preuves conformes, worktree propre. Voir `breadboard-B/remote-verification.json`.

---

## Historique jusqu'au checkpoint314ec129


# TR2-001 — module B éprouvé sur TR2-002, calibration toujours refusée

**IN_PROGRESS.** L'utilisateur confirme le transfert hors tension du module B,
celui des deux essais réussis sur TR2-002, vers le câblage conservé de TR2-001.
Module A mis à part ; VDD et VDDIO mesurés par l'utilisateur à **3,315 V**.

Contrôle D04 sans flash, reset ni START : UID001 conforme, BIN D01 de 50 912 octets
intégralement identique à l'image conservée, source **acb142794570ded5abd15f6521eb5d8b644590e8**,
SHA-256 `6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2`.
WHO_AM_I `0x7B`, réglage ±16 g relu `0x04`, SPI3 8 MHz. Le démarrage est déjà
arrêté sur **stage99/error7/run0 : refus de calibration**, avant le WAIT.
**Aucun essai de 60 s lancé et aucun appui USER demandé.**

Les 198 brackets de calibration sont sauvegardés : quatre lectures incohérentes,
résidu maximal **213 382 894 µs**, recalculé exactement hors ligne, limite inchangée
à 100 µs. Exemple : raw16 929 951 entre des voisins108 403 et110 008.
Les octets SPI originaux de ces lectures ne sont pas disponibles : la trace est
réinitialisée avant STOP dans calibrate(). Les valeurs sauvegardées démontrent
l'incohérence, sans déterminer sa cause électrique ou logicielle exclusive.

FIFO arrêté proprement, zéro erreur FIFO, writer inactif, CS SD haute,
**zéro commande/lecture/écriture SD**, pilote non initialisé et gate écriture fermé.
GDB détaché après préservation. Aucun accès TR2-002 dans cette tranche ; sa dernière
restauration reste acquise. Aucun changement FRAM, COM4, firmware, protections ou SD.
Aucun build ni test ancien relancé : tranche d'observation et d'analyse sans code modifié.

**Le module B fonctionne sur002 mais ne suffit pas à faire fonctionner001.**
Cela ne permet pas d'accuser le seul module A. Il reste à isoler fils/contacts,
alimentation effective et Nucleo001, sans prétendre que son MCU est défectueux.
L'ancien refus d'écriture SD est un problème distinct, non résolu par ce diagnostic.

Prochaine étape proposée : garder B sur001 et remplacer uniquement la liaison
capteur par les fils qui ont servi aux essais réussis sur002, hors tension,
sans modifier les réglages, le lecteur SD ou les jumpers. Action physique utilisateur
encore nécessaire ; aucun nouveau départ tant que le précontrôle n'est pas conforme.

[Preuves D04 et limites](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/module-cross/D04_RESULTAT_ET_SUITE.md).
HEAD de début de tranche : `c56872e1f0cc0cb83cf7d7269d999c702bf4c000`.
Journal actualisé avec le transfert rapporté, le refus, les preuves et la suite.
Checkpoint `3631037dc3604d79a6b479d61ff7ba64a4824680` publié par push normal sur main
et relu : 19 fichiers identiques, 594 empreintes de preuves contrôlées, worktree propre.
Relecture consignée dans `module-cross/remote-verification.json`.

---

## Historique jusqu'au checkpoint c56872e1


# TR2-001 — comparaison physique sans SD sur TR2-002

**IN_PROGRESS : comparaison terminée, localisation du défaut à poursuivre.**
L’utilisateur a explicitement autorisé cette extension le 02/11/2026 et confirmé
les deux bancs connectés en ST-LINK, avec la SD de TR2-002 insérée.
La décision est consignée dans les preuves ; le contrat ChatGPT reste inchangé.

| Diagnostic | Durée MCU | Sorties calculées | Contrôles temporels | Résidu maximal | Résultat |
|---|---:|---:|---:|---:|---|
| D01 / TR2-001, acquis | 16,659203 s | 44 288 | 167 | 228 049 169 µs | Refus error3 |
| D02 / TR2-002 | 60,000645 s | 158 450 | 599 | 19 µs | PASS borné |
| D03 / TR2-002 | 60,000639 s | 158 438 | 599 | 19 µs | PASS borné |

Sur TR2-002, les deux essais respectent la garde temporelle de 100 µs, les budgets
inchangés et les contrôles FIFO : zéro erreur FIFO, arrêt propre, zéro rail,
saturation, débordement de file ou masque de dépassement budgétaire.
**Aucune commande, lecture ou écriture SD**, pilote non initialisé et écriture
interdite, avant et après chaque essai. Aucun fichier créé : les nombres ci-dessus
sont des sorties calculées, pas des archives XYZ intégralement conservées.
Les snapshots et les digests sont sauvegardés, leurs CRC vérifiés.

Le profil comparateur conserve FS ±16 g, SPI3 à 8 MHz et FIR107 Q15/décimation 10.
Seuls la sélection UID et le départ RAM dédié diffèrent du diagnostic D01 ;
le profil autonome 001 conserve son appui USER obligatoire. GDB est armé au WAIT,
puis le CPU tourne sans pause jusqu’au breakpoint terminal après STOP/snapshot.
Deux essais au total, avec un reset logiciel entre eux pour vérifier la répétabilité.

Source évaluée : **6be825d4681a17224000ae1f2b088a315ba328c0**.
BIN : 50 928 octets, SHA-256
`7886feb2371837ceef7e126f26b4dedb2d484538a6142b8df2ccd662cc0dfbaf`.
Builds ARM comparateur et profil normal réussis ; configurations sans diagnostic
sans SD ou dépassant 60 s refusées. Les symboles d’initialisation, montage,
préflight et formatage SD sont absents de l’image liée. Pas de relance des anciennes
suites hôte inchangées. Le vérificateur de logs a d’abord manqué un retour à la ligne
CMake ; les refus effectifs ont été confirmés sans modification des contrôles.

**Préservation vérifiée :** identité SN/UID de TR2-002 contrôlée, deux sauvegardes
Flash complètes identiques et RAM initiale conservée. Image initiale N8G reconnue,
source `fbbdde5d328de47922cba688b6e406a327eb13f9`, 53 032 octets. Après les essais,
elle a été restaurée par flash standard. Le readback des **2 097 152 octets** de Flash
est strictement identique à la sauvegarde initiale, SHA-256
`113f54b740a2ee2be789ac42649becb8fe58b8a47d33890c587faabf405386e5`.
État final TR2-002 : N8G au WAIT, stage2/run0/error0, FIFO et writer arrêtés,
zéro commande SD, GDB détaché. Cette restauration ne requalifie pas N8G.
TR2-001 est inchangé : BIN D01 intégralement identique, ancien état99/error3,
FIFO et writer arrêtés ; aucun flash, reset ou START sur 001 durant la comparaison.
Aucune intervention FRAM, COM4, formatage, Option Bytes ou protection Windows.

**Conclusion limitée :** le défaut de compteur de TR2-001 n’est pas reproduit sur
TR2-002 pendant deux fenêtres de 60 s. La carte SD et son trafic ne sont pas
nécessaires à l’échec de TR2-001. Cela ne prouve ni une panne du MCU001, ni
l’innocuité absolue du logiciel partagé : capteurs, fils et conditions des deux
montages n’ont pas encore été croisés. Aucun signal électrique n’a été mesuré.
Aucune qualification d’endurance ou de stockage n’est acquise par ces diagnostics.

Prochaine étape proposée : échange croisé des modules capteurs, les deux bancs
hors tension, en conservant les câblages propres aux Nucleo ; puis un diagnostic
court sans SD sur 001. Cette intervention physique reste à effectuer par
l’utilisateur. Aucun troisième essai 002 ni départ 8 h dans cette tranche.

[Résultats détaillés et preuves](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/bank-comparison/RESULTATS_ET_SUITE.md).
HEAD initial de tranche : `663df57b`. Journal actualisé avec les résultats,
les limites, la restauration et la prochaine action. Commit et push normaux sur
main réalisés : checkpoint `1195af35571e3b9d4041894bcc70e5ef57e71d18` publié et relu depuis origin/main,
83 fichiers strictement identiques, 577 empreintes de preuves contrôlées,
CRC et source des deux snapshots distants vérifiés. Worktree propre à ce contrôle.
Preuve : `bank-comparison/remote-comparison-verification.json`.

---

## Historique des acquis jusqu’au checkpoint663df57b (états précédents)


# TR2-001 autonome — reprise de la qualification progressive des alimentations

**IN_PROGRESS : essais repris sur accord explicite utilisateur du 02/11/2026**
(« OK nous sommes alignés, tu peux débuter quand tu veux »), après discussion du plan.
La campagne autonome reste non qualifiée. L'analyse A1H et ses preuves restent
acquises ci-dessous ; la suspension qu'elle décrit est historique, levée par cet accord.

Plan : 2×60s sous ST-LINK seul, 1×60s CN15+ST-LINK avec GDB,
1×60s CN15+ST-LINK sans attachement GDB durant le démarrage,
1×60s CN15 seul, puis10min sur la configuration finale seulement si tout passe.
Arrêt au premier échec ; pas d'acquisition longue8h. Accord et périmètre consignés
sans modifier MISSION_EN_COURS.md dans
`preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/DECISION_ET_PLAN.md`.

**Diagnostic D01 sans SD : même erreur de compteur reproduite.** À la demande de
poursuite d'analyse et après proposition utilisateur d'échange de carte/banc,
préparation puis essai borné sur001 uniquement. Aucun accès à002/sa carte32Go.
Profil explicite `R3_TR2_001_SENSOR_ONLY`, OFFpar défaut, limité001/60s, excluant
l'initialisation SD et utilisant la session existante non-writer/run2. PiloteSD
noninitialisé et gateécriturefermée ; invariantzérocommande/lecture/écriture vérifié
avant départ et après arrêt. Aucun contrôle du compteur ou des budgets affaibli.
Cross-build du mode et du profil normal PASS ; configurations >60s/autrecible refusées.

Source **acb142794570ded5abd15f6521eb5d8b644590e8**. Douzième flash standard66,
BIN50912B/SHA **6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2**,
readback67 entier exact. WHO7B/FS04/calibration54µs/WAIT, capteur8MHz. USERphysique,
ST-LINKseul/GDBarméavantdépart, carte512auPC/lecteurTR2vide, CN15absent/JP6STLK/JP2off.
Breakpoint68 terminal seulement, après STOP/snapshot ; GDBdétaché puis lecture69.

Arrêt **16,659203sMCU**,99/error3,167checks, FIFOerror0/stoppedtrue, maskbudget0.
DernierFIFO **1489062**, précédentdirect **1481141**, lecturefautive **19689725** dans
le bracket **178243551–178243569µs**. Trace reçue **FD 70 2C 01**, HAL0/transfertcomplet.
Recalcul du résidu **228049169µs** exact/limite100. SnapshotCRC923a838d conforme,
SHA **06567836ce84183315691b83e06b0a116e5aa2ec40ba33d5b25fd13fcf5a9ed0**.
Lecture69 : **SDcommands=reads=writes=0, initialized=false, write_enabled=false**,
sensor_only=1, writerinactif. Maxima polls complets : service702,525µs/2500,
traitement373,16875/1000, intervalle962/5000 ; appel writer vide1,0125µs, pas transfertSD.
442872natifs acceptés/44288sorties calculées, digestRAM conservé ; **aucune archive
complète XYZ créée ou récupérée pour ce diagnostic sans stockage**.

**La carte SD et son trafic ne sont donc pas nécessaires à cet échec de compteur.**
Cela ne qualifie pas la512Go globalement ni ne résout le refusSD distinctA1D.
Le breakout SD/fils/alimentation restent présents, aucune forme d'onde capturée :
capteur, liaison, Nucleo et logiciel de transport ne sont pas départagés. Le driver
et le format32bits sont conformes aux référencesST consultées ; pas de correction
logicielle de bits ni de prétention de cause exclusive. Voir les preuves
`sensor-isolation/RESULTAT_ET_COMPARAISON_PROPOSEE.md` et `D01-failure-analysis.json`.

État final observé : 001 sousST-LINK seul, source arrêtée, GDBdétaché, lecteurvide,
SD512auPC ; firmwareD01diagnostic enFlash. Pas B02/CN15/10min. Le testcomparatif002
est maintenant pertinent, mais **le contrat l'interdit explicitement** : aucun flash,
debug ou geste sur002 sans extension humaine de périmètre. Plan borné proposé et
limites consignés dans la preuve précitée ; pas une qualification générale du firmware.

**Palier1 : B01 en échec après 17,088670 s MCU, sous ST-LINK seul.** L'observateur
est armé au WAIT (WHO7B/FS16, calibration16µs, capteur8MHz/SD4MHz), puis USER est
pressé physiquement. Le processeur tourne sans halt pendant la capture ; breakpoint
matériel atteint uniquement à `r3_finished`, après STOP et snapshot. Pas de reset ni
reconnexion du câble pour récupérer le résultat. Logs63/64 conservés.

Le snapshot67488B est valide, CRCc25b500d/source3f803d77, SHA
**c93226441712b19dfe8cdaa2f0141c5c67e19607104e8e18b132bf3cb2628492**.
`stage99/error3` : refus de cohérence du compteur. Dernier FIFO **1523255**,
précédent compteur direct **1515331**, lecture fautive **19823754** dans le bracket
**1049006171–1049006189µs**. La trace originale préservée avant tout reset contient
les octets reçus **8A 7C 2E 01**, qui décodent exactement cette valeur en little-endian.
La conversion numérique n'invente donc pas ce saut. HALstatus/error0, RX/TXrestants0,
cinq octets reçus incluant la commande, CSbas au contrôle avant fermeture ; ceci
ne constitue pas une mesure de forme d'onde ni une cause électrique exclusive.
Le recalcul indépendant reproduit **229404244µs** de résidu/limite100 inchangée.

Masque budget **0**. Maxima enregistrés des polls complets : service1446,79375µs/2500,
traitement720,75625/1000, intervalle1706/5000, SD456,39375/1000. Le passage fautif
renvoie sur l'horloge avant le calcul final de ses budgets ; aucune valeur manquante
n'est inventée. SDerror0/FIFOerror0/rails-clips0 ; STOP134µs, sourcearrêtée,
writerterminé/nonincertain/CS_SDhaute. GDBdétaché. L'échec de compteur ne nécessite
**ni CN15 ni l'absence de GDB** : l'hypothèse d'un défaut propre à CN15 est insuffisante.
Cela n'explique pas rétroactivement le refusSD A1D ou le dépassement de budget A1H.

Utilisateur transfère la512Go auPC et laisse001hors tension. Copie native complète
B010005.TR2, 2Mio, SHA **944b50e38613f7d30fc345fb1f71605fbec62faa6cd6197b07ca9f192a7c48d7**,
source=copie ; A1A–A1H re-hachés inchangés. Lecteur strict : INCOMPLETE576records/44928XYZ.
Ajout6recordsRAM vérifiés et30XYZpartiels : **45426sorties272556B**, SHA
**d1badcb64b64be828c97eaba5b03cf0f0e091168afce44329f9cbc498edc78a5**,
égal au digest producteur. Toutes les sorties filtrées produites récupérées, pas60s
ni tous les mots natifs. Preuves compactes, trace SPI active et suffixes publiables
sous `qualification-alimentation-20261102/`, originaux locaux dans le dossier homonyme.

**Arrêt de la comparaison au premier échec, conformément au plan.** B02 reste préparé,
non flashé/non lancé ; nouveaux paliersCN15 et10min non lancés. FirmwareB01 laissé
sur001 hors tension, JP6STLK/JP2retiré/CN15débranché, SD512 auPC selon utilisateur.
Étape suivante : traiter la lecture directe du compteur avant de reprendre les
comparaisons d'alimentation ; aucune garde supprimée/assouplie et aucune variante
à vitesses mixtes implémentée à ce point. Mission globale IN_PROGRESS, campagne
non qualifiée. Les états H/préparation ci-dessous sont historiques.

**Préparation logicielle de B01/B02** : source
**3f803d77393c6853ed76d796367df443a0eb9eb9**, diagnostic exact du budget temporel
(masque service=1/traitement=2/intervalle=4/SD=8, valeurs fautives et maxima conservés).
Expression OR de refus et seuils d'origine inchangés ; copie supplémentaire en RAM
après STOP. Test hôte ciblé frontières/violations combinées PASS, ARM ciblé PASS ;
aucune suite inchangée rejouée. Base commune de fréquences : **capteur8MHz/SD4MHz**,
comme l'essai A1C réussi ; FS16, filtre et garde horloge inchangés.

B01 et B02 : 60s USER, fichiers neufs B010005.TR2/B020005.TR2,
BIN53312B chacun, strictement identiques après remplacement du seul préfixe.
B01 SHA **7d9f2b59c312407273dc08ba8daa1e4758339259996beb1321b73aede66888d0** ;
B02 SHA **59a236ae0d8f3d8f6b492fc84914cbf33cc5eaa304aaf13eebdb4e222f41cffb**.
Préflight natif SD512/FAT32/cluster32Kio et absence des deux noms vérifiés en lecture
seule. Préparation physique demandée : SD auPC, lecteurTR2vide, CN15 débranché,
JP6STLK1–2/JP2retiré, ST-LINK seul. B01 a depuis échoué ; voir le résultat en tête.
Les informations de suspension/état matériel A1H ci-dessous sont celles du point
précédent, pas un état imposé après cette nouvelle intervention.

## 1. Résultat précis A1H

Snapshot réel de 67 440 octets conservé à travers le redémarrage observé lors de la
reconnexion ST-LINK : CRC **3ea13dd5** recalculé conforme, SHA
**461681043376180a6a522be624dd04e3b7987e2d69ea2c4d6be4dbbb0edff9a0**,
provenance source **65703ced5354d37724b2005bc0eba0fdad58d7dc**. Le relevé 59 distingue
l'ancienne capture du nouveau BOOT ; il ne transforme pas les compteurs remis à zéro
en résultat de l'essai. Voir `a1h-real-validation.json` et le journal GDB 59.

- `stage=99`, `error=6`, durée **0,279495 s MCU**, 7 032 natifs acceptés,
  704 sorties filtrées, arrêt FIFO confirmé ; aucune saturation ni clipping.
- Le code `poll()` renvoie -6 si service > 2 500 µs, intervalle > 5 000 µs,
  traitement consommateur > 1 000 µs ou quantum SD > 1 000 µs.
- Maximum SD retenu : **83 847 cycles / 160 = 524,04375 µs**. Cette garde SD
  n'est donc pas celle dépassée. Les maxima/dernières valeurs des trois autres
  budgets ne sont pas conservés : **impossible de distinguer service, intervalle
  ou traitement à partir de ce snapshot**. Aucun seuil n'a été relevé.
- Le contrôle d'horloge a effectué trois vérifications, avec écart maximal
  **63 µs / limite 100 µs**, mapper sans faute. Cela ne qualifie pas la fiabilité
  du compteur à 4 MHz sur 60 secondes.
- SD et writer : erreur 0, groupe terminé, réponse **0xE5 acceptée** (bits bas 5),
  aucune incertitude. FIFO : erreur/erreur STOP 0, arrêté ; niveau maximal 149,
  aucune file pleine. La vitesse réduite n'a donc pas réussi le critère court,
  mais aucune cause électrique exclusive n'est établie.

Analyse structurée : `a1h-failure-analysis.json`. Le matériel reste protégé par
les gardes ; pas de faux footer COMPLETE, de retry aveugle ou de correction de bits.

## 2. Données récupérées et préservation

Utilisateur : 512 Go transférée au PC, TR2-001 hors tension. Lecteur identifié
par capacité exacte **511 868 665 856 octets**, partition MBR/FAT32 vérifiée ;
la lettre D: relevée ce jour n'est pas un identifiant permanent. Copie native
intégrale `A1H0005.TR2` de 2 Mio, SHA source = copie :
**97b90e1b77b2f9e38adaacfb4a41ae32efea21ffe7c7a64023b472eaf40eba6f**.
Les fichiers A1A à A1G ont été re-hachés et sont inchangés. Aucune réparation,
aucun formatage ni écriture volontaire sur les fichiers source.

Lecteur strict : **INCOMPLETE**, huit records CRC/session/séquence conformes,
624 XYZ persistés. Ajout d'un record RAM également contrôlé et de deux XYZ
partiels : **704 XYZ / 4 224 octets**, SHA
**50be04e0308460ae21e86dd2cef3c6901a643963007097307da6f3ed4919b1e6**,
égal au digest producteur sauvegardé. Toutes les sorties filtrées produites de
cet essai sont récupérées ; cela ne signifie ni 60 s, ni tous les mots FIFO natifs,
ni durée SI indépendante, ni fichier COMPLETE.

Originaux et RAW dérivés :
`/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101/`.
Preuves Git : `preuves/TR2-20261101-001-AUTONOME/` ; extents utiles compressés,
suffixes RAM, snapshots, copies natives, lecteur strict et SHA256SUMS.
La queue préallouée inutilisée est exclue des extents publics : leur reconstruction
est dérivée, ce n'est pas une copie intégrale originale dont on inventerait le hash.

## 3. Synthèse des essais acquis — aucune répétition des réussites

| Essai | Configuration / observation | Résultat réellement établi |
|---|---|---|
| A1A | Gate USER 3 s, SPI3 8 MHz | COMPLETE, 7 958 XYZ, relecture PC et digest amont conformes |
| A1B | Powerbank seule, 60 s demandées | INCOMPLETE, préfixe PC 13 104 XYZ ; RAM perdue, cause initiale inconnue, alimentation ensuite mesurée à 0 V |
| A1C | ST-LINK, 60 s demandées | COMPLETE après 60,000418 s MCU, 159 686 XYZ, relecture PC/digest conformes |
| A1D | Secteur CN15 seul, SD 4 MHz | Refus SD_RESPONSE, état 6, après 10,804 s ; 28 758 sorties produites récupérées, digest conforme |
| A1E | Secteur seul, SD 2 MHz | INCOMPLETE, 27 456 XYZ persistés ; état terminal perdu au reset, pas de preuve de toutes les sorties produites |
| A1F | Snapshot conservé, SPI3 8 MHz / SD 2 MHz | Refus du suivi d'horloge à 11,271195 s ; 29 999 sorties produites récupérées, digest conforme |
| A1G | Compteur brut et mapper conservés, même vitesse | Refus du suivi d'horloge à 42,023136 s ; 111 917 sorties produites récupérées, digest conforme |
| A1H | Option SPI3 4 MHz existante / SD 2 MHz | Garde de budget à 0,279495 s ; 704 sorties produites récupérées, digest conforme |

Pour A1G, l'entrée incohérente est démontrée : FIFO brut **3 518 223**, précédent
compteur direct **3 510 360**, nouveau direct **23 748 123** dans le bracket MCU
**400 152 755–400 152 773 µs**. Avec l'état mapper retenu, le recalcul indépendant
prédit 653 206 637 µs et retrouve exactement **253 053 864 µs** de résidu rejeté.
Le rejet est correct pour cette observation ; la cause de l'entrée incohérente
(transport, lecture du compteur ou autre) n'est pas physiquement isolée.
Il ne faut pas remplacer rétrospectivement le refus SD réel A1D par cette autre cause.

## 4. Modifications, validations et limites

Profil expérimental TR2-001 isolé : FS physique ±16 g relu, échelle nominale
488 µg/LSB explicitement portée par le format v3 et le lecteur PC ; compatibilité
±8 g conservée. CID/capacité SD512 stricts, MBR/BPB vérifiés, FAT32 en clusters
32 Kio, libre contrôlé, noms neufs CREATE_NEW. USER avec relâchement préalable,
antirebond, maintien volontaire, départ unique ; WAIT, LED et arrêt autonomes.
Aucun START long envoyé par Codex. Le firmware de production longue n'est pas déployé.

Les snapshots de diagnostic sont en RAM NOLOAD hors BSS, avec provenance,
version/taille/CRC et publication du magic en dernier. Ils ont survécu à un test
synthétique de reset logiciel puis aux resets réellement observés sur F/G/H.
Aucune garantie après perte d'alimentation. Ils n'écrivent ni FRAM ni SD.

Les tests ciblés bouton, carte, lecteur ±16 g/compatibilité ±8 g, réponses SD et
CRC de rétention ont réussi ; builds ARM des profils modifiés réussis, compilation
du profil historique également contrôlée. Les sorties réelles sont conservées.
Le changement H est seulement l'option existante SPI3 4 MHz ; aucun test acquis
inchangé n'a été rejoué. Dernière analyse H : lecture RAM, copie native PC,
CRC/digests et inspection du code, **aucun nouveau flash/reset/essai**.
Le protocole de validation ciblée convient au harness expérimental ; la suite
complète inchangée n'aurait pas démontré une preuve matérielle supplémentaire.

Les échecs d'outillage sont conservés dans l'historique : build lancé avant fin de
configuration, indentation refusée puis corrigée, lecture d'adresse SPI3 erronée
au relevé 57 puis corrigée au 58 sans nouveau flash/reset. Aucune validation n'a
été obtenue en désactivant un contrôle. Les précédents checkpoints détaillés sont
préservés intégralement dans `HISTORIQUE_CHECKPOINTS_AVANT_ANALYSE_A1H.md`.

## 5. Alimentation, cible et état de reprise

Cible unique : **TR2-001**, ST-LINK **003500463235510B37333439**, UID
**004A003D/38425018/20333157**. Les deux sauvegardes Flash initiales de 2 Mio ont été
vérifiées avant le premier flash, SHA
**31926eb263db85917e7f4c04b95109c2fb174102100349a7da532ca3bd9807c5**.
Aucun mass erase, Option Bytes, FRAM, COM4 ni action sur TR2-002/sa carte32Go.

Pendant une transition ST-LINK, un nouveau flag PINRSTF a été mesuré sans nouveau
BOR ; JP2/T_NRST a ensuite été retiré hors tension par l'utilisateur. D'autres
reconnexions ont malgré tout été suivies d'un nouveau BOOT avec flag logiciel
présent ; leur cause exacte reste inconnue. Deux attachements GDB avec câbles
inchangés ont préservé les compteurs et flags. JP2 retiré ne constitue donc pas
une correction complète. La mesure utilisateur 3,23 V sans ST-LINK est ponctuelle
et n'exclut aucun transitoire. Le manuel ST confirme la fonction de JP2 :
https://www.st.com/resource/en/user_manual/dm00789949.pdf

**État final rapporté par l'utilisateur : TR2-001 hors tension, 512 Go dans le lecteur
PC, JP2 retiré, JP6 USB [7–8]. Firmware A1H laissé en Flash**, source
65703ced5354d37724b2005bc0eba0fdad58d7dc, 53 056 octets, SHA
**76241cc81a5a3e18144de40157ee9719112476f299392c652be74a0b1faf0c70** ;
readback intégral conforme au relevé 58, SPI3 CFG1=0x10070007/diviseur4.
Aucun debugger actif après détachement. Ne pas réutiliser les noms A1A à A1H.
Powerbank actuelle non qualifiée ; autonomie énergétique 8 h et acquisition
longue intégrée non démontrées. N8G de TR2-002 reste FAILED historiquement.

## 6. Git, publication et décision attendue

Worktree de mission : `/home/lolo/dev/msm/projects/MSM_vibe-TR2-001-autonome`,
branche locale `codex/tr2-001-autonome`, push autorisé vers `refs/heads/main`.
HEAD avant ce point : **7fa56863b62f1c4be9c7c5e4fddc6cbf45dda27e**, relu depuis
GitHub ; les preuves G publiques permettent aussi de reconstruire les sorties
récupérées et leur digest. Le rapport d'analyse a été publié au commit
**788b02428b054929ce3fc7fd78da82ce6b2a716d**, puis les 27 fichiers modifiés ont été
relus intégralement depuis la référence récupérée de GitHub : octets identiques,
394 empreintes de preuves conformes, CRC/provenance du snapshot vérifiés. La
reconstruction des 704 XYZ depuis les seules preuves distantes retrouve exactement
le digest producteur. Voir `remote-h-analysis-verification.json`. Le présent suivi
consigne cette vérification ; son SHA publié et son état Git seront donnés dans
la réponse Codex. Le statut de ce point historique restait IN_PROGRESS, avec essais suspendus pour discussion ; la reprise autorisée est décrite en tête du présent rapport.

Les anciens worktrees et le dépôt principal (HEAD82169af7) sont préservés.
Deux stashes toujours présents : 6251b988e8b20f97997a8f98fd26d4d35a45a1b2 et
f3b829e6188f491920edf74a5f9ed0f559d08b87. Aucun merge/rebase/reset destructif.

Journal actualisé et synthétisé pour cette mission ; corps historique de la
référence initiale conservé octet pour octet. Aucun autre code modifié à ce point.
Prochaine étape : examiner les idées de l'utilisateur avant tout nouveau changement
ou essai. L'option d'une lecture courte à vitesse distincte reste une **idée non
implémentée**, sans autorisation technique nouvelle déduite de ce rapport.
