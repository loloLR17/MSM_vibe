---
mission_id: TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR
status: IN_PROGRESS
base_ref: main
base_sha: a8817bf293c6e60a93ad824d279ee870b6a0ecf7
initial_head: 79b952dbb949cb8eb2f208e3b2c5994f4ed1279f
result_sha: 2a159954a7df0bc0e986985a7c93ff8793dfb435
created_at_utc: 2026-11-01T16:57:14.405923+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# TR2-001 autonome ±16 g — checkpoint ; qualification sans ST-LINK en cours

## A1F flashé — conservation RAM démontrée sur reset synthétique ; essai demandé

Utilisateur confirme CN15 puisST-LINK/JP2retiré/lecteurTR2vide, SD512 auPC.
Identité001/état précédent sûrs vérifiés46, **huitième flash standard**47 A1F/source2a159954,
BIN52940B/d4c7b699…2819d vérifié-v/-rst, readback48 entier exact. Pas de carte SD cible.

Pour qualifier le mécanisme nécessaire de conservation avant tout nouvel essai :
fixture **explicitement synthétique, pas des données acquises**, guard/version/taille/CRC
écrits seulement en RAM à20085ae0 ; **un reset logiciel** monitor reset, puis lecture49.
Les67224octets sont identiques avant/après, SHA
**31c351154fd3b40fc1381e436b1b29d7c66355c980f87ec16ff56a6e6192932e**,
validation CRC au boot/r3_retained_available=1, marqueurSYNTHETIC eterror52535431
conformes. Fixture invalidée après sauvegarde (magic0/available0), aucune écriture SD
ni FRAM. Preuve physique limitée au reset logiciel testé ; pas garantie coupure/BOR
ni preuve d'exécution du snapshot terminal réel avant prochaine capture.

GDB détaché, firmware arrêté au refus attendu SDabsente sans écriture. Demande utilisateur
de remise carte hors tension puis **CN15 seul/JP2retiré/WAIT→USER pour A1F60s**, fichier
neuf A1F0005.TR2 ; résultat en attente, pas encore essai réussi ni départ long.
Checkpoint9b627669 a préservé code préparé/préfixeA1E, nouvelle preuve à publier/relire.


## A1E sans JP2 — préfixe récupéré ; diagnostic A1F préparé

Utilisateur confirme JP2 retiré/CN15 seul, puis WAIT vert clignotant, USER pour60s,
erreur après environ10s. **Ce test n'est pas réussi.** À la reconnexion ST-LINK,
bleu observé : relevé43 montre un **nouveau BOOT** (run0/error0/writes0, calibration22µs,
lecturesFAT5825/CSR1c004400), **pas l'état terminal de l'essai**. RAM terminale perdue,
aucun token ni cause exacte déductible de ce snapshot. Le retrait de JP2 n'est donc
pas une correction complète du problème de reconnexion/diagnostic. Relevés44/45
sans toucher aux câbles : flags remis4400 puis toujours4400, IRQ78246→82820,
lectures26292→27875 ; les deux attachements GDB préservent ici l'exécution.

Transfert SD512 au PC effectué utilisateur, banc hors tension. Copie native complète
A1E2Mio : **53ea152900a9ded7a1be9a3880752c6bd6f937ba64e94caf30dd2bcca7114067**,
source=destination ; anciens A/B/C/D re-hachés inchangés. Lecteur strict : **INCOMPLETE,
352records CRC/séquence valides,27456XYZ/164736B, préfixe10,303861s MCU nominales**,
SHApayload **36f6c401ed932a38d2bab67aaa4e55a9382dbdc9db69f6f7cb92b5dc90e9f681**.
Pas de footer ni preuve de toutes les sorties produites, puisque digest RAM perdu.
La baisse SD2MHz n'a pas résolu le défaut. Extents utiles lossless publiés ; original
entier local, aucun formatage/réparation/writeback.

**Diagnostic A1F préparé, non flashé à ce checkpoint**, source
**2a159954a7df0bc0e986985a7c93ff8793dfb435** : snapshot après STOP en RAM NOLOAD,
avec version/taille/CRC et publication du magic en dernier. Contient result/writer/token,
file entière/partiel/header/digest et provenance. Conservé à travers le zéro BSS si la
SRAM survit au reset ; **aucune garantie après coupure**, pas de FRAM ni nouvelle commande
SD. Invalidation seulement avant USER suivant, pas de remplacement par un refus au boot.
Ajout strictement profil001 ; script de lien partagé inchangé.

Test hôte ciblé CRC/commit incomplet/taille/version/corruptions **PASS** ; ARM001 et
compilation profil défaut002 **PASS**, pas de tests réussis inchangés rejoués. Tentative
build avant finconfiguration refusée sanscache, puis warning indentation traité en erreur
et corrigé sans désactiver contrôle ; logs négatifs conservés. Linker : .bss finit
20085adc, **.tr2_retained NOLOAD 20085ae0–20096178**, hors zéro startup et zone peinte
de pile (guard étendu),67224octets retenus. BIN52940B/SHA
**d4c7b699fe94fd59ae89810dac46f0a0908f6138b1098dee3e092be41772819d**,
A1F0005.TR2/60s/SD2MHz/SPI3 8MHz/FS16/gardes inchangés. Conservation réelle au reset
et prochain refus à qualifier ; pas de solution SD ni firmware longue campagne déclaré prêt.


## Transition contrôlée — reset NRST démontré, couplage JP2 à vérifier

Utilisateur : **3,23 V** au lecteur SD pendant le retrait du ST-LINK, CN15 conservé ;
mesure ponctuelle, pas exclusion d'un transitoire. Reconnexion rapportée bleu clignotant
puis fixe. Relevé42 : **CSR0x04004400**, comparé à0x00004400 remis à zéro en40 :
**nouveau PINRSTF seulement**, aucun nouveau BOR/SOFTWARE/WATCHDOG. IRQ38478
contre103846 avant transition, initialisation SD désormais réussie/999743488 secteurs,
FAT32 monté, calibration27µs/198brackets/6628ancres, WHO7B/FS04, error0/write0 ;
BOOT/f_getfree en cours, pas encore WAIT/START. Il y a eu un reset par NRST pendant
la séquence physique de retrait/reconnexion ; son front exact n'est pas mesuré.

Documentation officielle ST UM2861 Rev10, tableau5 : **JP2/T_NRST relie/isole le reset
du ST-LINK à la cible**. C'est une hypothèse matérielle discriminante, pas une cause
définitivement démontrée. Le passage sur l'alimentation externe3V3 recommande aussi
son retrait contre les resets du ST-LINK non alimenté ; ce passage ne doit pas être
confondu avec notre alimentation CN15. Source :
https://www.st.com/resource/en/user_manual/dm00789949.pdf
Prochaine action : repérage JP2 sans déplacement alimenté, fin contrôle SD puis
isolation hors tension et contrôle CN15 seul avant essai court. Aucun firmware changé,
aucun reset/flash/START/écriture SD commandé par l'agent pendant ces relevés.


## Reprise — rouge après reconnexion : refus SD avant acquisition

Utilisateur : bleu environ une seconde à la reconnexion ST-LINK puis rouge clignotant
à nouveau. Lecture38 sans reset/flash : **Error_Handler/stage99/error2=SD_TIMEOUT**,
WHO7B/FS04 conformes, **aucune calibration ni capture ni écriture**, FAT non monté.
CMD0 et CMD8 ont répondu correctement (idle1/echo01AA), puis ACMD41 reste idle1
sur plusieurs centaines de commandes et finit sans réponse (R1FF). Ce n'est ni une
preuve de besoin logiciel du ST-LINK, ni le précédent refus d'écriture A1D.
Le retour aux variables initiales depuis36 montre un nouveau démarrage entre relevés ;
son instant et sa cause ne sont pas déterminés.

Lecture39 : Flash52 448 octets exactement identique au BIN A1E attendu,
CFSR/HFSR=0, LD3/PG2 effectivement haute à cette phase de clignotement ; aucun
HardFault signalé. RCC_CSR réel **0x46020CF4=0x0C004400**, BOR/PIN cumulés, non
attribuables au dernier branchement. L'adresse0x46020C94 dans38 est AHB3ENR,
**pas CSR** ; aucune interprétation reset de cette adresse. Après sauvegarde, lecture40
remet uniquement les flags volatils à zéro via RMVF (équivalent HAL documenté),
CSR relu0x4400, IRQ103846, erreur2 inchangée, SDwrite0 ; aucun reset effectué.
Débogueur détaché. Une seule transition physique ST-LINK seul retiré/réinséré avec
CN15 conservé et mesure3V/GND du lecteur est demandée à l'utilisateur pour comparer
les nouveaux flags et la continuité du compteur. Pas de retry SD ni d'essai A1E lancé.


## Checkpoint A1D / A1E — refus SD reproduit sur secteur, données récupérées

**IN_PROGRESS**, pas de firmware longue campagne déclaré prêt. A1A 3 s et A1C 60 s
restent acquis avec lecture PC intégrale ; leurs essais réussis ne sont pas répétés.

**A1D, secteur CN15 seul pendant la capture : échec réel après 10 804 000 µs MCU.**
Le relevé 32 après reconnexion ST-LINK, sans reset, indique error95, writer SD_RESPONSE3,
état6/bloc1 du groupe LBA257065, réponse non acceptée, uncertain/needs_power_cycle vrais.
Aucun retry ni footer COMPLETE. FIFO arrêté, CS relâchée ; aucun nouvel ordre SD envoyé
avant le cycle physique demandé. FIFO/horloge/rails/clips sans faute observée. La valeur
brute du token n'était pas mémorisée dans cette image : aucune cause électrique ni
erreur CRC sur le fil n'est établie. Ce refus sur secteur interdit d'attribuer tous les
échecs à la powerbank.

La copie native Windows intégrale A1D (2 Mio) est INCOMPLETE, 360 records CRC valides,
28 080 XYZ, SHA du préfixe 32153cc8…e3058. Les huit records retenus en RAM et 54 XYZ
partiels permettent de récupérer **tous les 28 758 XYZ filtrés produits**, 172 548 octets,
SHA **be5cbd92b83411106b18eacc99a2ac0ea91f8362f844ffbaaa8a5f4c434d327c**,
égal au hash producteur finalisé depuis son état RAM sauvegardé (finaliseur vérifié sur
A1C). Pas 60 s, pas les natifs intégraux, pas de faux footer. A/B/C relus inchangés.
Preuves publiques : extents utiles compressés D, suffixe XYZ de 4 068 octets, état de hash,
file RAM et rapports ; les gros originaux sont conservés localement.

**Candidat A1E**, code 0751b0604108ccae780074feaf7513cebf2975bc : SPI2 SD à **2 MHz**
(diviseur8 au lieu4), SPI3 capteur inchangé8MHz, FS16/488µg, token de réponse conservé
et digest/erreur conservés même en fin fautive, en RAM uniquement. Acceptation
(r&31)==5 et arrêt sur incertitude inchangés. Option isolée 001, défaut002 inchangé.
Test hôte ciblé des tokens acceptés/refusés et verrou de non-retry **PASS MODEL** ;
build ARM candidat **PASS** ; compilation ARM profil historique par défaut **PASS**,
sans flash/debug sur002. Résultats conservés, aucune répétition de ces validations.

Septième flash standard : BIN52 448 octets, SHA
**c88e8c69622a33ab27de1e6b7c5cf3c94774adb998c616b8b58bfc7a169a99f0**,
préfixe neuf A1E, 60 s USER seulement. Relevé36 : UID001, WHO7B/FS04, calibration31µs,
BOOT/error0/run0/write0, FAT32 monté/comptage libre en cours ; SPI2 CFG1=0x20070007
confirme diviseur8, PG2/LD3 bas. Ce relevé précède le dernier retrait du ST-LINK :
**l'utilisateur rapporte ensuite LD3 rouge clignotante dès retrait**. Pas de WAIT ni
de USER confirmé ; observation à confronter au prochain relevé RAM, sans supposer
que LD3/LD4 sont confondues ni que l'acquisition a démarré. Secteur demandé maintenu,
aucun redémarrage/appui demandé pendant son absence.

Reste : identifier cet état sans ST-LINK, qualifier A1E court en autonomie et relire
intégralement son fichier PC si capture, puis seulement déployer l'image longue
R1A au WAIT. Aucune acquisition longue déclenchée, aucune autonomie8h certifiée.
Powerbank actuelle non qualifiée ; disponibilité de l'alimentation stable au réducteur
à confirmer. TR2-002, FRAM, COM4, anciennes données, worktrees et stashes préservés.


## Reprise22UTC — 60s STLINK intégralement relues, alimentation autonome encore à résoudre

**A1C PASS cible etPC, sans répétition de l'essai réussi.** SurSTLINK/JP6STLK :
60000418µsMCU/1596856natifs→159686filtrés/958116B utiles,0rails/clips/FIFO/SD/filefaute,
FIFO52/file5120B/work735,5125/service1461,8125/gap1721µs, seuils inchangés.
STOP134µs/drain14840µs, FIFOoff/queue0/writeroff/nonincertain ; observermatérielarrêtfinal
uniquement/detach. Relecture native WindowsUSB (disque1/D: à22:07) :copie2Mio entière
source=destination **3560b45ce97627a31ac68b29ac19bcc198e076d828378f6808b2ef4ec8044950**,
**2048records/159686XYZ COMPLETE** etSHAamontRAM
**d29e04f2219dac04ff6d6523f49d0ce49a02ac82f898b639e2f1983d4620bd07** exact.
A1A/A1B re-hachés inchangés. Extents utilesC lossless publiés, queuepréallouée exclue.
Cette relecture achève la preuve du fichier C ; pas d'autonomieénergétique8h implicitement.

**ComparaisonB/C** : BIN52416B strictement identiques après remplacement des seules
constantesSHA40octets/préfixeA1B→A1C ; aucun algorithme ou garde modifié. Cela ne
sépare pas à lui seul sourcephysique/debugger/montage ; échecB conservé sanscauseinventée.

**A1D diagnostique60s préparée/non démarrée**, nouveaupréfixe/codeC4f152eb,
sixièmeflashstandard de mission-v/rst, effectué SDabsente après sauvegarde C.
UserSDremise/JP6USB4epaire[7–8]/CN15bankA→C+STLINK confirmé, aucunUSER.
GDB30 :STLINK détecté maisMCUabsent. UserLD4rougeseule/**0Vlecteur**, JP6sur4epaire
confirmé. Réveilbankuser :LEDs5/7/2 annoncées transitoires, ne restentpas ; pas d'acquisitionD.
Ce sont des faits d'alimentation au repos ; ni code d'erreur initial B ni causeélectrique
exclusive n'est établi. Aucun changementGPIO/câblage/OptionBytes/protection.

À la propositionuser, mêmeCN15 alimenté USB-A T1000 (JP6USB/STLINKprésents) :MCU
accessible, maisbootSDrefus3/CMD0R1=0,0write/0cal/0capture ; snapshot31 sauvegardé,
aucune attente protocole assouplie. Changementde source sans coupurefroide prolongée
n'est pas une preuve de défaut du firmware ; un démarragefroid complet reste nécessaire.
User propose désormais une **alimentationsecteurUSB-C5V**. Vérificationautonome sur
sourceconstanteprévue, banque actuelle nonqualifiée ; utilisationsecteur pourcampagne
soumiseàla confirmation de disponibilité au réducteur. Pasd'imageprodlongueflashée,
pasREADY8h niSTARTlong. Tousfichiersdéjàacquis sauvegardés, SDsansécriture àl'état31.

## Relecture PC acquise — 3s COMPLETE ; essai powerbank INCOMPLETE ; diagnostic A1C en cours

**Natif Windows, lecteur USB Generic STORAGE DEVICE, disque2/D: à ce relevé**
(et non supposition F:/ancien32Go) :511868665856B/MBR/offset1048576/partition511867584512B,
volume **FAT32 «512 GO»/cluster32768**. Copies intégrales des deux fichiers et hash natifs
source=copie exacts, pas de formatage/réparation/écriturefichieroriginal. Originaux locaux
`native-pc-files`, données sourcepréservées, 32Go002 nonouverte paroutilcopie.

- **A1A0005.TR2 COMPLETE** : tous103records/7958XYZ/47748B, CRC/séquence/session/temps/
  profil16g488µg/footer vérifiés, SHAamontRAM91a4…72e6 exact ; fichierentierSHA
  **1370b795a465f1c1df07a3fdeca9e16b1df967483534f94a43d26a9e27dc761e**.
  Toutes7958sorties produites de ce court essai sont persistées. Conversion nominale
  de tous les échantillonsréels hors11warmup disponible dans `physical-fs16-nominal-scale.json`
  (DCconservée/vecteurmoyen proche1g, pas calibration absolue).
- **A1B0005.TR2 INCOMPLETE** :168records valides/13104XYZ/78624B, intervalle des sorties
  **4921139µsMCU**, aucunfooter ; SHA du seulpréfixe **2b3a5508335fc8fd673dfc3c24388f5a19c9e59cb727e6834bf18eb1c26da0c5**.
  Fichierentier2MioSHA **9bb593f8ed83e972c685b2e49411df6429c1016659ddfeeca1051259b8139a16**.
  Pas60s validées ni comparaisonàunSHAproducteurcomplet (RAMperdue/inaccessible),
  pas de garantie de récupération de toutes les sorties initialement produites.

User rapporte LD3rouge avant extinction, puis a réveillé la powerbank ; mesure ensuite
**0Vaulecteur**, STLINKprésent maisMCUinaccessible (`22…No device found on target`).
La première erreur et sa causalité (source d'alimentation/SPI/logiciel) restent inconnues ;
ne pas attribuer automatiquement l'échec à une mise en veille de banque. Une seule
powerbank disponible selonuser. Essai autonome60s **échoué/nonqualifié**, pasREADY8h.

Preuves compactes lossless publiées : extents en-tête/recordsvalides/footer +descripteurs
et zlib ; tousoctets inutilisés de préallocation sont exclus. Reconstruction clairsemée
**dérivée**, pas prétention auSHAdufichieroriginalentier ; lecteurstrict relu surcesextents :
A1ACOMPLETE/7958/SHAamontexact, A1BINCOMPLETE/13104/SHApréfixeexact. Deuxoriginaux
entiers restent locaux. Cela permet le contrôleindépendant des données utiles publiées.

**Diagnostic sousSTLINKstable** : userSDretour001/JP6STLK/CN15off ; nouveaupréfixeA1C,
sourceC885e505, aucune modification ducode/gardes, ARM60sPASS, cinquièmeflashstandard
-v/rst. Refus initial avantcalibration : CMD0/R1=0 au lieu1, SDerror3/writes0. Cardétait
reluparancienfirmwarelorsduflash ; aprèscoupurecomplèteuser, **initSD/FAT32/calibration
réussissent**, booterror0. Refus conservé distinct, aucune attente de réponse assouplie.
L'observateurinitial25 a été fermé parSIGINT/GDBcode1 carattachéalorsqueError_Handler
déjàatteint ; lecture26 a conservé lesfaits sansreset. Observateur27corrigé vérifie
l'étatterminalavantcontinue, breaks matériels fintraitement/entréeError_Handler, aucun
START, collectefinalepuisdetach. Actuellementarmé ; USERdemandéuniquementaprèsWAIT,
STLINKmaintenu pour préserverle premierdéfaut. **Pasencorede résultatA1C annoncé.**
C885checkpointgate précédent relu130fichiers/112empreintes à17:47:43UTC.

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
