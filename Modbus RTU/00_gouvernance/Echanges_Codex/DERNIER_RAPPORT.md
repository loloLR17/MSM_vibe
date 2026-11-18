---
mission_id: TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF
status: IN_PROGRESS
base_ref: main
base_sha: dcd3be60e147d610e171ff2ca37f099e35054dfe
initial_head: fb7167548dc1f55eb8250d3d52c8c85dd5caa8c9
result_sha: dc4ac0414db6795796a1649a9433d4f543d8a310
created_at_utc: 2026-11-18T21:51:12.377899+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield V2.4 — correction SPI3 et CAM candidat prêts pour revue

**La correction CAO et ses contrôles numériques sont réalisés. Le ZIP reste CANDIDAT_NON_LIBERE.** Le statut IN_PROGRESS porte à présent sur publication/relecture distante, pas sur une attente de test matériel. Aucune commande ni opération STM32 n'a été exécutée.

## Livrables

- [Dossier V2.4 et projet KiCad éditable](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/README.md), schéma/bibliothèque/PCB/routes corrigés dans une variante distincte.
- [ZIP candidat unique](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip), [SHA externe](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip.sha256), [inventaire des9CAM](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/CAM-manifeste.json).
- [SchémaPDF](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/schema.pdf), [implantation](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/implantation.png), [cuivreavant](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/cuivre-dessus.png), [cuivrearrièremiroir](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/cuivre-dessous-miroir.png), PDF/SVG vectoriels associés.
- [E-01/M-01 détaillés et limites](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/CONTROLES_ET_LIMITES.md), [180contacts](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/E01/MATRICE_E01_180_CONTACTS.csv), [17nets](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/E01/MATRICE_E01_17_NETS.csv), [diffV2.3/V2.4](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/diff-V23-V24.json), outils de comparaison et sorties natives dans le dossier.
- [Achats urgents](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/ACHATS_URGENTS.md), [BOMnative19repères](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/BOM_NATIVE_2_SHIELDS.csv), [liste exacte de préparation](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/achats/PREPARATION_2_SHIELDS.csv), inventaire utilisateur et sources commerciales datées.

Code/CAO réellement évalués **dc4ac0414db6795796a1649a9433d4f543d8a310**. PCB SHA256 **0492d95930c5d5eabdea8c526dcc225c196e5edf5c9807a055ac5f0dc6ee4a56**. ZIP SHA256 **d2a199a1bf3b9d9533bfe788b144510bb8cfd518dc248f05df2c2a644854e7ec**. SeptGerber/deuxExcellon, aucun ZIP ancien ou fichier source mélangé aux9membres. Les noms natifs et marquages historiquesDRAFT/V1.9/ARNACO sont préservés, l'identité correctiveV2.4 est explicite dans dossier/ZIP/manifest. Le gbrjob informatif reste horsZIP ; contour axial90×100mm, épaisseur1,6mm, deuxcuivres35µm inchangés. Sa taille90,05×100,05 inclut le trait, `Finish:None` n'est pas un choix de finition de commande.

## Correction et contrôles observés

| Tranche | Résultat |
|---|---|
| J8 SPI3 | PC9/CS→19 ; PC10/SCL→20 ; PC12/SDA→21 ; PC11/SDO→22 ;23/24NC ;1/2/13rails inchangés |
| Modification native | SixcontactsJ8 seuls changent de net ;54segments terminaux retirés/45reroutés sur les4netsSPI3 ; préfixes, tous autres segments et4vias conservés |
| ERC/DRC KiCad10.0.6 | **0violation,0non-connecté,0parité** ; règlesprojet et listes ignorées strictement identiques àV2.3, aucun contrôle affaibli |
| Référence E-01 reconstruite | **180/180PASS**,59contacts reliés,121NC voulus et17nets ; références fabricants indépendantes du projet avant comparaison |
| Symboles/bibliothèques |180pins externes/embarquées/XML comparées, directions des signaux connectés contrôlées, nomsCS/SCL/SDA/SDO corrigés |
| CAM géométrique | Deux vrais cuivres reconstruits,180flashes parface,138composantes ; **0court-circuit,0coupure** ; tous trousPTH/NPTH/vias comparés aux Excellon |
| CAM horscuivres |7fichiers équivalents àV2.3 hors seules dates/LF ; aucune coordonnée/commande de tracé retirée de la comparaison |
| M-01 natif |19empreintes,191géométries pads/trous,4vias et tous paramètres mécaniques identiques ; contour/stack/épaisseur/sérigraphies inchangés |
| M-01 gelV2.7 |232fichiers historiques conservés byteexact ; lecture seule duSTEP336solides,191interfaces de perçage et3enveloppes sockets recoupées, G12,25 inchangé |
| Contrôles négatifs |8mutations interdites rejetées en mémoire, sans modifier le PCB : position, via, perçage, épaisseur, logo, J9, segmentRTU, fauxnetACC |

Les vrais cuivres sont analysés par unions de flashes/traits/régions puis composantes reliées seulement par les trous métallisés et4vias ; les attributsX2 ne remplacent pas ce calcul. Le lecteur refuse les commandes Gerber inconnues. RésolutionCAM1µm et approximation descercles128segments ; pas une métrologie usine.

Validation proportionnée : pureCAO/documentation/achats, aucun build ou test firmware justifiable. Les anciens essais RS485/FRAM/SD/capteur et les validations STL inchangées ne sont pas rejoués. Les nouveaux contrôles portent uniquement sur la correction et la préservation des interfaces. V2.3 et V2.7 ne sont ni retouchées ni réorganisées. GOUVERNANCE/AGENTS/contrat humain inchangés.

## Sources, incidents et réserves

Les8sources sont épinglées dans [reference-constructeurs.json](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/E01/reference-constructeurs.json) : ST C05 natifPCAD/PcbDoc/projet et UM2861, EagleAdafruit aux commits exacts, ST MKIGIBV2 et Clickv102. La page schématique officielleST version1/feuille1/Fig2 est consultable viaoutilWeb ; deux téléchargements directs ont expiré. Transcription visuelle recoupée au dessinSTDB4012Rev3/p2/Fig2 conservé via miroir haché ; **aucun hash du PDF officiel local n'est inventé**. Les réserves de provenance sont explicites.

Clickv102 est la source électrique ; le v101 sert uniquement à la géométrie de ses16contacts, sans identité matérielle globale prétendue. Les niveaux/straps/périphériques réels restent des gates du futur assemblage, pas des observations de cette mission.

Trois incidentslogiciels ordinaires sont conservés dans [INCIDENTS_OUTILS.md](../../04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/preuves/INCIDENTS_OUTILS.md) : formatKiCad10 de nets parnom initialement supposé numérique ; deuxextrémités routesJSON à1nm du natif pour la sélection des segments ; `pinfunction`XML en `nom_numéro`/`+no_connect` au lieu du nombrut. Corrections fondées sur les sorties réelles ; M-01 compare ensuite les coordonnéesexactes. Aucun échecERC/DRC n'a été masqué.

**Réserve mécaniqueJ5 supplémentaire** : le STEP simplifié mesure7,68mm longitudinalement, contre8,13mm au Samtec. Le corps constructeur complet a été confronté au reste de l'assemblage gelé en mémoire : aucune intersectionpositive, aucun STL/STEP corrigé. Budget d'insertion nominalSamtec4,48mm dans3,68..6,35. Tolerances/forces/tenue et montage PLA ne sont pas physiquement qualifiés.

## Achats : stock pris en compte et délai non garanti

Inventaire utilisateur :1barre mâle2×20,6femelles1×12,4femelles1×10,6femelles1×8,2résistances et1condensateur de chaquecapacité ; **Samtec non reçus**. PrécisionJ5 : sur l'assemblage déjà testé, deuxtronçons1×3 issus de1×10 côte à côte, pas un Samtec. La qualificationfonctionnelle RS485 reste valable pour cet assemblage ; elle ne démontre pas une référenceSSW ou sa tenue. Archives précédentes non réécrites.

Plan minimal préparé : **2barres2×40,10résistances10k (palier moinscher que6),1AVX100nF,1Panasonic10µF et2Samtec exacts àapprovisionnementconfirmer**. Aucune femelle1×12/1×8 ni module complet à racheter. Les4femelles1×10 entrent dans les enveloppesV2.7 prévues, avec contact10 àneutraliser au futur montage9trous ; aucun geste exécuté. AVX/Panasonic sont des options historiques numériquement contenues dans les enveloppesgelées, distinctes des MPN natifs. CourtyardC1 plusétroit conservé explicitement ; queues/standoff/pose demeurent réservés. Les connecteurs soudés ne sont pas présumés récupérables.

Sous-total des4lignesTME : **3,76USDHT**, hors taxes/port/J5, pas un totalTTC inventé. TMEaffiche les lignes ; [expressFrance](https://www.tme.eu/fr/how-to-buy/7307/Moyens-et-couts-de-transport/)1–2jours ouvrables/19,08€TTC avant14hCET sousconditions. Commandejeudi19 peut arrivervendredi20 oulundi23 : **réception avant samedi21 non garantie**. DigiKey/Mouser : aperçus/caches puis refus d'accès, prix/stock/délaiSamtec actuels non démontrés. Aucun achat, paiement, connexioncompte, envoi au fournisseur ou engagement de délaiPCB.

## État final et publication

Premier checkpoint achats **b08f479d84020a7fa635a45b4ca766a1ae329418**, publié et6fichiers relus intégralement depuis GitHub avant poursuite. Candidat et preuves conservés au commit **dc4ac0414db6795796a1649a9433d4f543d8a310**. Worktreeisolé `MSM_vibe-SHIELD-V24-SPI3-CORRECTIF`, branche `codex/shield-v24-spi3-correctif`, ciblemain ; racinehistorique82169af7 et deuxstashes6251b988/f3b829e6 préservés. Aucun merge/rebase/reset/clean/forcepush. Les instantanés mission/rapportRS485 ont été archivés parChatGPT avant cette mission.

Journal `ETAT_COURANT_TR2.md` actualisé avec correction, résultats numériques, inventaire/J5 réel, réserves et suite. **À transmettre à ChatGPT : revue indépendante E-01/M-01 du nouveau candidat puis décisionutilisateur avant fabrication ; aucun GO usine délivré par Codex.** Le NO-GO desV2.3 physiques, montage/intégration futurs et étatsfirmware/données restent inchangés ; aucun accèsSTM32/COM/média dans cette mission. Publication complète et relecture des octetsZIP distants restent la dernière étape avant passageDONE.
