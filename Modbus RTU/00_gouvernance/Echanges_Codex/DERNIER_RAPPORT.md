---
mission_id: TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR
status: IN_PROGRESS
base_ref: main
base_sha: a8817bf293c6e60a93ad824d279ee870b6a0ecf7
initial_head: 79b952dbb949cb8eb2f208e3b2c5994f4ed1279f
result_sha: 94cb3b9570554c16b93c9d2272dcdee7db55ec7c
created_at_utc: 2026-11-01T16:57:14.405923+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# TR2-001 autonome ±16 g — checkpoint logiciel ; carte SD non initialisée

## Gate USER3s terminé — résultats cible acquis, PC et powerbank à vérifier

**OBSERVÉ cible/USER/LED : PASS borné3s, pas encore relecture nativePC.**
User : LD1verteclignotante→appuiUSER→LD2bleueclignotante→LD1vertefixe.
Relevé19/source94cb/BINf343…d5c6 : stage4/error0/standalone4,
3000012µsMCUnominales pour3000000demandées,79576XYZnatifs→7958filtrés,
47748B utiles/SHAproducteur **91a4ec489c07edbd34282b97d034568e0c4bcc417f5544301ebfe8d93d5c72e6**.
A1A0005.TR2 préalloué131072B, premierLBA248192/256secteurs ; FS16/488µg,
103records/0rails/0clips/0FIFO-SD-file-erreur. Freebefore15617113clusters,4requis.
Marges cycles/160 : service1359,75µs/work666,9125/gap1618/SDquantum440,11875,
seuils1000/2500/5000inchangés ; FIFO48/file5120B. STOP134µs/drain12492µs,
FIFOstoppedtrue/writeroff/uncertainfalse/filevide/CShaute/GDBdétaché.
CRC en-tête/footerRAM et digestconformes, Flashrelueentièreexacte. **Ce n'est pas encore
une preuve du contenu du fichierSD entier** : relecture nativePC conservée obligatoire.

BPB réel :512B/secteur,64secteurs/cluster,2FAT/122009secteursFAT,
total999741376secteurs/root2 ; label du champBPB `NO NAME` (étiquette de volume
racine/Windows non encore relue, ne pas la déduire de ce champ). Aucun formatage.

Image autonome60s/A1B nouvellement chargée-v/rst aprèscollecte3s : source50e47473,
BIN52416B **99dc1f5f01e9508445e0948153d4a46170a5e2cc2e82761c9881b51032fcd7e7**,
quatrièmeflashstandardmission, mêmesfonctions/guards. Demande physique : attendreWAIT,
couperalimentation, JP6de5V-STLKvers5V_USB_C[7–8] uniquement, powerbankUSB-A5V→CN15,
STLINKdébranché ; aprèsnouveauWAIT, USER2s→60s→vertefixe et observationtension.
**Retour utilisateur encore attendu : aucun résultat powerbank annoncé.** Aucun
nouvelappui logiciel/GDB, pas de STARTlong. Lecture intégrale des deux fichiers sur
lecteurPC puis firmwarelongR1A/WAIT restent àréaliser. Candidatlong compilé et conservé
localement/nonflashé, source50e etpréfixeR1A, guards30600sMCU/640Mio inchangées.

## Reprise de qualification — FAT32 monté, gate USER3s en préparation

Utilisateur demande de terminer la mission. Binding exactCID/capacité ajouté au commit
**94cb3b9570554c16b93c9d2272dcdee7db55ec7c** ; contre-tests mauvaisecapacité/CID et
ancien32GoPASS, les autres tests déjà acquis non répétés. BuildARM3s/A1APASS,
BIN52420B **f343a813fdad6fe568402954bb2ac69d37d0b70583d1a16c954cda8c1730d5c6**.
Troisièmeflashstandard de mission, UID001 contrôlé/CLI-v/rst réussi ; pas de resetextra.

Relevé `18-gate-boot-status.log` : **FAT32 réellement monté** (fs_type3), deuxFAT,
secteur512/clusters64secteurs=32768B, volbase2048/fatbase3790/database247808,
122009secteurs parFAT/n_fatent15617121. Calibration sûre±16gPASS :198brackets,
6623ancres/résidu57µs sous100, FIFOoff ; WHO7B/FSreadback04.

Au relevé : standalone_state1/initialisation, f_getfree encore en cours vers96768
lectures (~79%de la premièreFAT), aucunefautelogicielle/SD/horloge et zéroécriture.
LD2bleuefixe rapportée paruser, conforme au fait que le comptage bloque encore
l'entréeWAIT ; ne pas confondre LD5alimentation et LD1verte. Le boutonUSER ne sera
pris en compte qu'après le contrôle intégral libre/nom. **Aucun START à ce relevé.**
Appui physique USER2s demandé seulement après LD1verteclignotante, puis observation
bleu→vertefixe/rouge et collecte après arrêt ; aucun autre test pendant la capture.

JP6 repéré par l'utilisateur sur **5V-STLK**, aucun changement autorisé en activité.
Après gate/récupérationRAM : préparer variante60s neuve pour CN15/powerbank et suivre
UM2861/JP6USB[7–8] hors tension, après contrôle des bornes paruser. NativePCcardread et
imagefinaleWAIT restent à faire, aucunecampagnelongue lancée. État en préparation,
pas un résultat de capture ni une qualificationendurance. Historique conservé dessous.

## Dernière vérification — SD répond après remplacement du CLK, lecture seule

**Blocage de réponse SD levé dans la portée demandée : initialisation et lecture.**
L'utilisateur a remplacé le fil CLK puis confirmé ST-LINK reconnecté, SD insérée et
CN15 débranché. Première tentative avant cette reconnexion : aucun ST-LINK détecté,
pas d'accès cible ; seconde lectureGDB réussie, identité001 vérifiée. Aucun nouveau
flash/reset logiciel, aucun test hôte/build répété, aucune acquisition ou écritureSD.

Firmware de découverte56fa3b18 conservé, mêmes contrôles. Résultats observés :

- SD initialisée, CMD0 répond1, ACMD41 aboutit0 ; CID/CSD lus avec CRC, CMD59 actif.
- CID SPI **03534453443531328562e43cf80195eb**, CSD **400e00325b59000ee5b77f800a4040b5**.
- **999743488secteurs512octets =511868665856octets =476,71484375Gio**.
- Secteur0 lu avec contrôleCRC16 réussi, signatureMBR55AA, partitionunique type0C
  (déclaration FAT32LBA), début2048/longueur999741376secteurs, dans la capacitéCSD.
  **Le type de partition ne démontre pas encore le BPB ou le montageFAT32.**
- SDerror0/CRCerrors0/reads1/writes0, initializedtrue/failedfalse/write_enabledfalse.
  Stage99/error96 = arrêt **volontaire du probe avant gate d'écriture**, conforme au
  code, pas un défautSD ; diagnosticdistinct des précédents refus2/CMD0 sans réponse.
- WHO7B/CTRL1=04 conservés (FS16/ODRoff), GDB détaché. Pas de campagne démarrée.

Le retour de communication suit le remplacement du CLK et la reconnexion ; cela
étaye cette piste sans démontrer une causalité électrique exclusive ni qualifier
l'écriture, la capacité intégrale, le montageFAT32 ou l'endurance de la carte.
Conformément à « vérifie juste la SD card », aucune préallocation ni capture n'est
entreprise dans cette vérification. Contrôle de continuité demandé auparavant n'est
plus le verrou immédiat. Suite de mission : binding exact de ce CID/capacité, BPB et
montage/libre/noms, puis gatecourt/PC/powerbank/imageWAIT restent à réaliser.
Preuves : `15-clk-replaced-reconnected.log`, `sd512-restored-readonly.json` et MBR512B
exact dans `preuves/TR2-20261101-001-AUTONOME` ; les refus historiques restent intacts.

## Dernière reprise — alimentation lecteur mesurée, silence SPI persistant

**État actuel : pas prêt pour la campagne, contrôle de continuité utilisateur attendu.**
Utilisateur : le fil **+3V** était déconnecté, reconnecté ; **3,28V entre3V/GND sur
le lecteur Adafruit** ; brochage réaffirmé CLK CN7-5/SO CN9-10/SI CN9-8/CS CN7-20.
L'utilisateur a ensuite débranché/rebranché le ST-LINK et remesuré3,28V. La tension
pendant la coupure n'a pas été rapportée. Le silence persiste après ce redémarrage.

Un second flash standard justifié pour obtenir une trace bornée du tout premier échange,
source **56fa3b18e0ed9def3f67d19dfbe06f596b248e94**, BIN51640B SHA256
`a64337db036a72b9636c2888077b27cbc273bdc5975e84c380eaefbc42df1caf`, CLI-v puis
relectureGDB intégrale exacte. Aucun flash de l'image longue ni acquisition. Cette
image distincte de découverte conserve les64premiers appels I/O, au plus16octets
TX/RX par appel, hors boucleRX HAL. Aucun délai, contrôle ou protocole n'a été assoupli.

**Observation actuelle capteur : WHO0x7B et CTRL1_XL=0x04**, donc champ FS±16g relu,
ODRoff ; ce résultat lève la réserve de réglage physiqueFS16, pas la qualification
d'une acquisition ou de l'échelle de données réellement enregistrées. Le lecteur
Python/échelle488µg reste validé par fixtures seulement tant qu'aucun fichier n'existe.

`12-discovery-io-result.log` et `13-after-confirmed-cold-boot.log` :
CMD0 **40 00 00 00 00 95**, GPIOF_ODR PF12=0 pendant l'appel, HAL_OK, sixoctetsRXFF
pendant la commande puis aucune R1 recevable ; error2/R1FF/writes0/CIDinconnu.
Les10octets initiaux fournissent les80clocks demandées par le code avecCS haute.
Ce sont des **buffers HAL et états MCU**, pas des captures électriques au logementSD :
présence réelle de SCK/MOSI/CS au lecteur non démontrée, aucune conclusion matérielle
exclusive. Décodeur du log natif, sans réinterprétation des caractèresWindows, dans
`first-command-observation.json`. Aucun diagnosticN8G ni essai d'endurance.

**Prochaine action physique demandée** : TR2-001 hors tension, contrôle au multimètre
continuité lecteur→Nucleo CLK/CN7-5, SO/CN9-10, SI/CN9-8, CS/CN7-20, GND/CN8-11,
carte verrouillée ; laisser hors tension et rapporter les coupures éventuelles.
Le brochage annoncé est conforme au contrat, aucune modification n'a été prescrite.
Débogueur détaché/SDsansécriture/ODRoff. Les validations hôte et les builds autonomes
acquis n'ont pas été rejoués ; seul le build de ce diagnostic modifié a été réalisé.

Checkpoint précédent **faa1f7b713c60737bffd3d92e24715a5aaa256f5** récupéré deGitHub et
relu intégralement à2026-11-01T17:17:50.241886Z :66fichiers/49empreintes conformes,
worktree alors propre. État courant publié ci-dessus ; les sections suivantes conservent
l'historique des premiers checkpoints et leurs limites à leur date.

## Historique du premier checkpoint

**Pas prêt pour la campagne.** Logiciel autonome préparé et builds ARM courts/long
réussis ; première découverte physique : WHO_AM_I **0x7B**, mais **CMD0 sans réponse
(R1=0xFF, SD_TIMEOUT2)**. Aucun CID/CSD/capacité/MBR512Go obtenu, zéro écritureSD,
aucun enregistrement. Ne pas confondre le build logiciel avec une qualification.
L'utilisateur a confirmé la coupure/reconnexion001 et la correction d'un fil
Dupont d'alimentation déconnecté. Le refus initial restant verrouillé, un seul reset
logiciel GDB autorisé a réarmé la découverte : **CMD0/R1FF/SD_TIMEOUT2 persiste**,
WHO7B et zéro écriture. Tension directement sur Adafruit4682 et nature du fil corrigé
(+3,3V/GND) demandées ; retour attendu. Poursuite sous le même contrat.

## Préservation et identité

- Worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-TR2-001-autonome`, branche `codex/tr2-001-autonome`,
  démarrage propre depuis origin/main79b952d. Dépôt historique/main82169af et worktree
  R3a8817bf préservés, aucune résolution de leur divergence. Deux stashes historiques
  6251b988 et f3b829e6 inchangés. Aucun reset/clean/restore/rebase/merge/force push Git.
- Seule cible physique utilisée : SN003500463235510B37333439,
  UID004A003D/38425018/20333157 relu par GDB avant flash. TR2-002 hors opérations.
- Les deux sauvegardes utilisateur2Mio ont été relues et hachées effectivement avant
  flash : SHA256 `31926EB263DB85917E7F4C04B95109C2FB174102100349A7DA532CA3BD9807C5`.
  Emplacement Windows `C:\Users\lolo\Downloads\MSM_TR2_Backups\TR2-001_20261101` ;
  `backup-verification.json` reproduit les tailles/hashes observés, pas un hash supposé.
- L'inventaire Windows montre la carte historique32Go dans le lecteur natif ; aucun
  fichier de cette carte n'a été ouvert. Utilisateur confirme séparément **512Go dans
  TR2-001, câblage SPI2 et polarité/3,3V vérifiés**. Le FAT32/label512 GO reste une
  information utilisateur non vérifiée via le banc à ce stade.

## Réalisé en logiciel

Commits `64ee78c410659b6fdc44c78f96c2516bfd877888` et
`6c5ecb47e0c7bdbf83f9541d585a79d3dc9aaf24` : profil explicite001 dans `experiments/r3_durable_fat32`,
réemploi FIFO/FIR107Q15 /10/v3, SPI3duplex8MHz ; profil002 et V1 conservés.

- FS logique3/physique±16g, 488µg/count dans en-tête et lecteur ; les anciennes données
  ±8g restent244µg/count. FS16 n'est accepté que dans v3, sans nouvelle signification
  implicite des formats antérieurs. Rails/clips, CRC, SHA producteur, séquences,
  session, temps, footer et seuils1000/2500/5000µs conservés. DC conservée ici.
- Boot sûr→initialisation/capteur/SD/calibration→contrôle espace/nom→WAIT ; USER/PC13
  entrée pulldown, debounce50ms et appui1500ms après stabilisation, relâchement requis,
  verrou de départ unique, aucun START RAM ou à l'alimentation dans le profil autonome.
  LED verte WAIT, bleue ACTIVE, verte fixe COMPLETE et rouge ERROR préparées d'après
  BSP officiel ST. Arrêt FIFO tenté en cas d'échec calibration ; traces de défaut
  conservées. Ces transitions et LED restent à observer matériellement.
- STOP/flush/footer autonomes, durée cible longue30600000000µsMCU ; pas de garantieSI,
  pas de dépendance QPC/UART/Windows/GDB. Préallocation640Mio longue et tailles réduites
  pour gates3s/60s. Un nom existant est refusé, jamais réutilisé automatiquement.
- Gate carte512 exact prévu dans `src/tr2_001_card.h` : **binding zéro volontaire** tant
  que CID/capacité inconnus, donc aucune écriture autorisée. MBR FAT32 simple/type0B/0C,
  BPB512octets/cluster/plage contrôlés avant gate ; pas d'exFAT ou fallback32Go.
  Formatter désactivé dans001. Ni cette géométrie ni le libre n'ont été mesurés sur512Go.

## Validations effectivement exécutées

Validation proportionnée au harness isolé : builds CMake ARM dédiés ; pas de suite
historique générale ni de `tr2_validate.sh` canonique sans apport pour ce contrat court.
Aucun test déjà acquis M014/R3/N8G rejoué. Les branches par défaut du backend restent
inchangées ; le lecteur a ses tests ciblés et un oracle exhaustif de l'ancienne échelle.

1. Trois tests hôte dédiés PASS : bouton/rebond/maintien au boot/appuis répétés ;
   MBR/BPB/ranges/overflow/exFAT/mauvaise identité ; parsing/échelles FS8 etFS16/CRC.
   Fixtures synthétiques seulement : pas de qualification physique512Go.
2. Extension du test lecteur après restriction v3 : neuf cas + les65536int16 de l'échelle
   FS8 comparés à l'ancienne conversion, PASS (`host-reader-extended.log`). Les deux
   autres tests inchangés n'ont pas été répétés sans raison.
3. Build ARM final source `6c5ecb47e0c7bdbf83f9541d585a79d3dc9aaf24` PASS pour gate3s/A1A et
   variante longue/R1A **non flashées**. LongBIN51540B SHA256
   `5896c473e8d571c60bcddcdaec7779cfab17ab89e8a2d21773ed06f2e746ad40`.
   Tailles/ELF/BIN/CMakeCache et SHA dans `artifact-manifest.json` ; fichiers volumineux
   conservés localement, pas Git. Absence des symboles f_mkfs/witness_uart/r3_command
   contrôlée dans les deux images autonomes. Warnings nosys de newlib consignés,
   aucun warningC ignoré ; un premier échec misleading-indentation corrigé et conservé.

## Première observation physique, distincte du logiciel préparé

Un flash standard de découverte **lecture seule**, adresse0x08000000, CLI `-v -rst`,
SN001 explicite, téléchargement vérifié. Aucun mass erase/OptionBytes/déprotection.
Image51068B SHA256 `1283f70a60b68e7de03298014f3c23f79d987c72023dad1b26839a8b6eb224bc` ;
base79b952d avec adaptation de découverte alors non commitée, **pas** l'image autonome
finale6c5ecb47. Son BIN a été préservé par relectureGDB et hash intégral exact.
Trois sessionsGDB : identité préalable, observation, préservation readback ; détachées,
port qualifié23336. Au premier checkpoint : aucun reset logiciel supplémentaire. À la reprise : un seul
`monitor reset` justifié ci-dessus, sans flash ni START.

`03-discovery-card.log` : WHO123=0x7B, CPU160MHz, stage99/error2,
last_cmd0/R1FF, initializedfalse/write_enabledfalse, writes0/CRCerrors0.
**L'absence d'erreur CRC n'est pas une validation CRC puisque la carte n'a pas répondu.**
Aucune mesure de FS16 ni calibration/gate d'enregistrement dans cette tentative.
Cause du silence SD non établie : état résiduel, contacts/câblage/alimentation ou autre
restent des hypothèses. La coupure/reconnexion et le réarmement après correction ne suffisent pas à obtenir
une réponse. Le prochain contrôle est la tension au lecteur, pas un nouvel essai aveugle. État conservé : firmware découverte arrêté sur erreur,
aucune écritureSD en vol, GDB détaché ; pas d'image longue prête en Flash.

## Powerbank, limites et suite immédiate

Utilisateur : sans marque22000mAh, utilisation envisagéeCN15. Procédure préparée dans
[TR2_001_AUTONOME.md](../../../experiments/r3_durable_fat32/TR2_001_AUTONOME.md),
références primaires ST UM2861/BSP. PositionJP6/révision réelle non confirmée,
aucun jumper déplacé et aucun essai powerbank réalisé. Vérifier la sérigraphie avant
instruction physique CN15/JP6[7–8] ; ne pas utiliser CN1chargeur avec un hôtePC.
Ni capacité huit heures ni maintien en faible charge ne peuvent être certifiés par
la seule indication22000mAh ou un essai60s. N8G resteFAILED et sa cause non résolue ;
aucune immunité de001 ni endurance8h de la chaîne partagée revendiquée.

Ordre de reprise, sans refaire les tests acquis :

1. Obtenir la tension réelle3V/GND au lecteur et la nature du fil corrigé, déjà
   demandées. GPIO/MMIO lus via les définitionsCMSIS : SPI2 PB13/PD3/PD4 etCS PF12
   conformes, CS haute, défautHAL absent ; cela ne démontre ni continuité ni tension.
   Les étiquettes de code de l'ELF plus récent ne sont pas utilisées pour interpréter
   l'ancien binaire ; seuls types/BSS vérifiés par UID et registresMMIO sont lus.
   Toutes les sessions terminées/détachées ; pas de nouvel essai d'écriture.
2. Si CID/CSD accessibles, relever capacité/MBR/BPB, lier uniquement cette512Go,
   monterFAT32 et contrôler libre/nom/LBA ; sinon conserver le diagnostic et préciser
   la vérification de liaison électrique nécessaire, sans changer de carte par défaut.
3. Build/flash/readback de l'image courte liée, WHO/FS16/calibration puis gate3s ;
   qualifier USER/LED, éventuellement60s powerbank sans ST-LINK, relecture nativePC
   complèteCRC/temps/qualité/SHA. Tout transfert de carte/câblage reste utilisateur.
4. Image longue à préfixe neuf, retourWAIT réel et runbook final ; **départ long par
   l'utilisateur uniquement** après installation. Publier résultats et relectureGitHub.

## Publication et journal

Ce checkpoint IN_PROGRESS conserve les acquis, il ne clôture pas la qualification.
Journal `ETAT_COURANT_TR2.md` actualisé avec préparé/observé/non vérifié et prochaine
action physique ; ancienne synthèse N8G conservée intégralement. Preuves légères dans
`preuves/TR2-20261101-001-AUTONOME`, originaux locaux `/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101`.
Premier checkpoint publié par push normal `HEAD:refs/heads/main` :
`8c5157a0a5dc0799550110d46e7a1e7fa24c1f60`, relu à2026-11-01T16:59:01.939646Z :
44fichiers comparés intégralement et27empreintes des blobsGitHub vérifiées. Les logs
Windows sont conservés octet pour octet (CRLF/espaces natifs), sans normalisation
masquant les sorties. Cette mise à jour ajoute uniquement les observations de reprise ;
son SHA distant sera fourni après push et relecture. Aucun accèsCOM4, écritureFRAM, opération002 ou modification de protection.
