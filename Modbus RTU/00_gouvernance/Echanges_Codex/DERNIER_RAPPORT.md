---
mission_id: TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR
status: IN_PROGRESS
base_ref: main
base_sha: a8817bf293c6e60a93ad824d279ee870b6a0ecf7
initial_head: 79b952dbb949cb8eb2f208e3b2c5994f4ed1279f
result_sha: 3f803d77393c6853ed76d796367df443a0eb9eb9
created_at_utc: 2026-11-02T06:40:32.196709+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

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

**B01 chargé, acquisition non encore lancée.** Utilisateur confirme CN15 débranché,
JP6STLK [1–2], JP2 retiré, ST-LINK seul et lecteur TR2 vide. Identité/état sûr vérifiés
au relevé60 ; onzième flash standard61 de mission, vérification-v/-rst puis readback62
entier strictement identique : 53312octets/7d9f2b59…88d0. WHO7B/FS04, registre SPI3
CFG1=00070007/diviseur2 ; SDabsente/refus2 attendu, aucune écriture. Snapshot67488B,
available0. GDB détaché. Remise carte hors tension sous ST-LINK seul demandée, attente
WAIT sans USER avant armement de l'observateur terminal. Aucun résultat60s B01 acquis.

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
JP6STLK1–2/JP2retiré, ST-LINK seul. Aucun résultat de capture B01 à ce point.
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
