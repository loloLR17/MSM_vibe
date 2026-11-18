---
mission_id: TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF
status: DONE
base_ref: main
base_sha: dcd3be60e147d610e171ff2ca37f099e35054dfe
initial_head: fb7167548dc1f55eb8250d3d52c8c85dd5caa8c9
result_sha: dc4ac0414db6795796a1649a9433d4f543d8a310
verified_remote_sha: 2ad3852b4e87af7b24f98d871f59efcd69aa0969
verified_at_utc: 2026-11-18T21:57:53.653079+00:00
created_at_utc: 2026-11-18T21:51:12.377899+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield V2.4 — correctif SPI3 prêt pour revue indépendante

**La correction et les contrôles numériques sont terminés. Les Gerber restent CANDIDATS NON LIBÉRÉS pour fabrication.** Le statut DONE clôt cette mission de correction CAO, de préparation des achats et de publication vérifiée ; il ne libère ni fabrication ni assemblage physique. Aucun achat ni accès au matériel n'a été effectué.

## Livrables et identité

- [Projet et mode de reproduction](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/README.md) : KiCad éditable, symboles, bibliothèques, PCB et sources de correction dans une variante électrique distincte.
- [ZIP candidat unique](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip), [SHA-256 externe](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip.sha256), [inventaire des neuf membres](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/CAM-manifeste.json).
- [Schéma PDF](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/schema.pdf), [implantation](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/implantation.png), [cuivre avant](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/cuivre-dessus.png), [cuivre arrière miroir](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/cuivre-dessous-miroir.png), exports vectoriels PDF/SVG associés.
- [Contrôles et limites](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/CONTROLES_ET_LIMITES.md), [matrice de 180 contacts](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/E01/MATRICE_E01_180_CONTACTS.csv), [matrice de 17 nets](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/E01/MATRICE_E01_17_NETS.csv), [diff V2.3/V2.4](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/diff-V23-V24.json), sorties brutes et comparateurs dans le même dossier.
- [Achats urgents](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/ACHATS_URGENTS.md), [BOM native](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/BOM_NATIVE_2_SHIELDS.csv), [préparation pour deux shields](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/PREPARATION_2_SHIELDS.csv), inventaire utilisateur et sources commerciales datées.

La CAO effectivement évaluée appartient au commit **dc4ac0414db6795796a1649a9433d4f543d8a310**. Les corrections documentaires de publication ne changent pas le PCB ni les CAM :

- PCB SHA-256 : `0492d95930c5d5eabdea8c526dcc225c196e5edf5c9807a055ac5f0dc6ee4a56`.
- ZIP SHA-256 : `d2a199a1bf3b9d9533bfe788b144510bb8cfd518dc248f05df2c2a644854e7ec`.
- Sept Gerber et deux Excellon ; neuf membres, aucun ancien ZIP ou fichier source mélangé. Les cartes de perçage et le gbrjob informatif restent hors ZIP.

Les noms natifs et marquages historiques V1.9/DRAFT/ARNACO sont préservés par M-01. L'identité corrective V2.4 est portée par le dossier, le ZIP et les manifestes. Le contour axial reste 90 × 100 mm, épaisseur 1,6 mm, deux cuivres de 35 µm. La taille 90,05 × 100,05 du gbrjob inclut le trait ; `Finish:None` n'est pas une sélection de finition de commande.

## Modifications et résultats observés

| Contrôle | Résultat |
|---|---|
| J8 SPI3 | PC9/CS → 19 ; PC10/SCL → 20 ; PC12/SDA → 21 ; PC11/SDO → 22 ; 23/24 NC. Rails 1/2/13 inchangés. |
| Diff natif | Six contacts J8 changent de net ; 54 segments terminaux retirés, 45 reroutés sur les quatre nets SPI3. Préfixes, autres pistes et quatre vias conservés. |
| KiCad 10.0.6 | **ERC 0 ; DRC 0 ; non-connectés 0 ; erreurs de parité 0.** Règles et listes de contrôles ignorés identiques à V2.3, aucun contrôle affaibli. |
| E-01 constructeur | **180/180 PASS**, 59 contacts reliés, 121 NC et 17 nets. Référence reconstruite depuis les sources fabricants avant comparaison au projet. |
| Symboles | 180 broches comparées entre bibliothèque, schéma et XML ; types des signaux connectés et noms CS/SCL/SDA/SDO contrôlés. |
| Cuivres CAM réels | 180 flashes par face, 138 composantes ; **zéro court-circuit et zéro coupure**. Tous les trous Excellon PTH/NPTH et vias comparés. |
| CAM hors cuivres | Sept fichiers équivalents à V2.3 après seules normalisations des dates de création et fins de ligne. |
| M-01 natif | 19 empreintes, 191 géométries de pads/trous, quatre vias, contour, stack, épaisseur et sérigraphies exactement conservés. |
| M-01 V2.7 | 232 fichiers historiques intacts ; STEP gelé de 336 solides relu, 191 interfaces de perçage et trois enveloppes de sockets recoupées. G = 12,25 mm inchangé. |
| Contrôles négatifs | Huit mutations interdites rejetées en mémoire : position, via, perçage, épaisseur, logo, net J9, segment RTU, faux net ACC. |

Le lecteur de CAM reconstruit les unions géométriques de flashes, traits et régions des deux vrais cuivres. Les traversées ne sont réunies que par les trous métallisés/vias ; les attributs X2 ne remplacent pas ce calcul. Les commandes Gerber inconnues provoquent un refus. Résolution CAM de 1 µm et cercles approchés par 128 segments : cette comparaison n'est pas une métrologie de fabrication.

Les commandes et sorties réelles sont conservées dans [native-commands.json](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/docs/checks/native-commands.json), [export-commands.json](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/docs/checks/export-commands.json), les journaux ERC/DRC et les preuves E-01/M-01. Validation proportionnée à une mission CAO/documentation : aucun build firmware ni ancien essai physique rejoué, aucun STL/STEP/kit Morvan régénéré. AGENTS, gouvernance et contrat humain sont inchangés.

## Provenance et réserves techniques

Les [huit sources constructeur](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/E01/reference-constructeurs.json) sont identifiées par URL/version/hash lorsque disponibles : ST C05 natif PCAD/PcbDoc/projet et UM2861, Eagle Adafruit aux commits exacts, MKIGIBV2 et Click v102. Le schéma officiel ST version 1/feuille 1/Fig. 2 a été consulté par l'outil Web ; deux téléchargements directs ont expiré. La transcription visuelle est recoupée au dessin ST DB4012 Rev. 3/p. 2/Fig. 2 conservé via miroir haché. **Aucun hash d'un PDF officiel local absent n'est prétendu.**

Click v102 est la source électrique ; v101 ne sert qu'au contrôle de la géométrie de ses 16 contacts. Cela ne démontre pas la révision réelle du module. Straps, niveaux, références et montage effectifs restent à contrôler à froid lors de l'assemblage futur.

Les [incidents d'outillage et de publication](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/INCIDENTS_OUTILS.md) sont conservés : format des nets KiCad 10 initialement mal supposé, sélection de deux extrémités JSON décalées de 1 nm du natif, conventions `pinfunction`/`pintype` XML. Les comparateurs finaux contrôlent les coordonnées exactes ; aucun échec ERC/DRC n'a été masqué.

**J5 : réserve de représentation mécanique.** Le STEP simplifié mesure 7,68 mm longitudinalement contre 8,13 mm au Samtec. Le corps constructeur complet a été confronté en mémoire au reste de l'assemblage gelé : aucune intersection positive. Aucun modèle imprimé n'a été modifié. Insertion nominale Samtec de 4,48 mm, dans la plage documentée 3,68–6,35 mm ; les tolérances, efforts, maintien et montage PLA restent non qualifiés physiquement.

## Stock réel, achats préparés et délai

Stock NON SOUDÉ rapporté par l'utilisateur : une barre mâle 2×20, six femelles 1×12, quatre femelles 1×10, six femelles 1×8, deux résistances et un condensateur de chaque capacité. **Les Samtec n'ont pas été reçus.**

Le J5 du montage déjà testé est constitué de **deux tronçons femelles 1×3 côte à côte, adaptés de 1×10**. L'acquis physique RS485 de cet assemblage est conservé ; il ne démontre ni la présence d'un Samtec, ni l'équivalence mécanique de cette adaptation. Les archives précédentes ne sont pas réécrites.

Préparation minimale proposée pour deux nouveaux shields :

| Composant à acheter si décision utilisateur | Quantité | Référence |
|---|---:|---|
| Barre mâle 2×40 | 2 | Connfly DS1021-2*40SF11 |
| Résistance 10 kΩ | 10 | Vishay MRS16000C1002FCT00 ; palier moins cher que six unités |
| Condensateur 100 nF | 1 | AVX SR205C104KAR |
| Condensateur 10 µF | 1 | Panasonic EEUFR1H100 |
| Femelle J5 2×3 exacte | 2 | Samtec SSW-103-01-F-D ; approvisionnement non confirmé |

Aucun rachat de femelles 1×12/1×8 ou de module complet proposé. Le plan de découpe des mâles n'a aucune réserve : une troisième barre ne serait qu'une option de précaution. Les quatre femelles 1×10 entrent numériquement dans les enveloppes V2.7 prévues pour J6/J7, avec dixième contact à neutraliser au futur montage à neuf trous. Leur profondeur utile n'est pas transférée à J5. Aucun geste matériel exécuté.

Les options AVX/Panasonic et 1×10 restent distinctes des MPN de la BOM native. Corps contenus dans les enveloppes gelées ; courtyard C1 plus étroit explicitement réservé, queues/standoff/pose à contrôler. Les connecteurs déjà soudés ne sont pas présumés récupérables.

Sous-total indicatif des quatre lignes TME : **3,76 USD HT**, hors taxes, port et J5 ; ce n'est pas un total TTC en euros. [Transport TME France](https://www.tme.eu/fr/how-to-buy/7307/Moyens-et-couts-de-transport/) : express affiché 1–2 jours ouvrables, 19,08 € TTC, avant 14 h CET et sous conditions. Une commande jeudi 19 peut arriver vendredi 20 ou lundi 23 : **réception avant samedi 21 non garantie**. DigiKey/Mouser : aperçus/caches puis accès refusés ; prix, stock actuel et délai du Samtec non démontrés. Aucun achat, paiement, accès à un compte fournisseur ou envoi de fichier effectué.

## Git, publication et continuité

- Départ réel : `fb7167548dc1f55eb8250d3d52c8c85dd5caa8c9` ; référence du contrat : `dcd3be60e147d610e171ff2ca37f099e35054dfe`.
- Premier checkpoint achats : **b08f479d84020a7fa635a45b4ca766a1ae329418**, publié et six fichiers relus intégralement depuis GitHub avant poursuite.
- Lot CAO : **dc4ac0414db6795796a1649a9433d4f543d8a310** ; rapport provisoire : **1078d465b5ec50d2a40497ceb789acc8c7addc24**.
- La vérification d'inventaire après cette publication a détecté le ZIP omis par `*.zip` de `.gitignore`. Ajout explicite du seul ZIP candidat, sans modifier les exclusions ni régénérer le lot. Le `.kicad_prl` local est conservé mais retiré du manifeste de sources, car état GUI hors livrables.
- Worktree isolé `MSM_vibe-SHIELD-V24-SPI3-CORRECTIF`, branche `codex/shield-v24-spi3-correctif`, push normal vers `main`. Racine historique `82169af7`, travaux précédents et deux stashes `6251b988`/`f3b829e6` conservés. Aucun merge, rebase, reset, clean ou force push.
- Journal `ETAT_COURANT_TR2.md` actualisé : correction, preuves, stock réel et J5 adapté, réserves et prochaine décision. Les instantanés mission/rapport RS485 avaient été archivés par ChatGPT avant la présente mission.

**Publication complète relue depuis GitHub : `2ad3852b4e87af7b24f98d871f59efcd69aa0969`**, le 2026-11-18T21:57:53.653079+00:00. [Preuve détaillée de relecture](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/publication-first-readback.json) : **126 fichiers relus intégralement**, comparés octet par octet aux fichiers publiés ; manifeste de 123 livrables vérifié, 45 hashes de sources recoupés et 232 fichiers historiques inchangés. Le ZIP issu des octets distants a été ouvert : neuf membres, CRC et SHA-256 de chacun conformes, identiques aux CAM séparés publiés. Aucun diagnostic CAO ou matériel répété pour cette relecture.

Au point relu, HEAD local et `origin/main` sont identiques, sans modification non commitée dans le worktree de mission. La présente clôture documentaire fera l'objet d'un dernier push normal et d'une vérification distante séparée ; son SHA est fourni dans la réponse finale, sans référence circulaire dans ce commit. Les seules permissions exercées sont les modifications CAO/documentaires, validations numériques, commits et push autorisés sur `main`. Aucun accès STM32, COM ou média ; firmware, données et acquis RS485 inchangés.

**Suite à transmettre à ChatGPT :** revue indépendante E-01/M-01 du candidat publié, puis décision avec l'utilisateur avant fabrication. Aucun GO usine délivré par Codex. La V2.3 physique reste défectueuse ; l'assemblage et la qualification complète futurs restent à réaliser.
