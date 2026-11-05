---
mission_id: TR2-20261105-SHIELD-V20-EMBOITEMENT-3D
status: DONE
base_ref: main
base_sha: 0ba0df1653bf8f7cac33f8b30ec0dba06517643a
initial_head: ce531d1e106affd3546a425947d0fb55cb185213
result_sha: 0854d7c71ee3946ae7d1b2ced962cd87a9a7722e
created_at_utc: 2026-11-05T12:24:21.743129+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.0 — emboîtement possible sous bornes, NO-GO de libération

**GO numériqueXY conservé ; faisabilité Z conditionnelle autour deG12,25mm ; gatephysiqueBLOCKED et fabricationNO-GO. Aucune nouvelle impression demandée.** Le papier ne lèverait pas l’engagementZ, les tolérances et les efforts. Étude terminée, publiée et relue depuisGitHub ; aucune commande, interventionmatérielle ou modificationPCB.

[Décision courte et argumentée](../../04_Architecture/SHIELD_PROTO_V1/V2_0_EMBOITEMENT/DECISION.md), [sources exactes](../../04_Architecture/SHIELD_PROTO_V1/V2_0_EMBOITEMENT/SOURCES.md), [coupes dimensionnées et plan](../../04_Architecture/SHIELD_PROTO_V1/V2_0_EMBOITEMENT/cad/assemblage-et-coupes.pdf). Dossier : `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_0_EMBOITEMENT/`.

## Résultats et limites

STEP officielC05 réellement importé,1060solides, PCBZ−1,6..0 et socketsZio8,51. ZIPSTBDP4.0/Manufacturing3.4 **rehachés**, conformes aux empreintesV1.9 ba9ce535…/ecd9d543… ; révisionC/assembly05 et coordonnéessources reprises sans contestation. Le nouveau dessinSamtec révisionCM a été téléchargé et consulté, empreinte conservée : hauteur.335in8,51±.005in≈0,13mm ; jeu d’insertion catalogue3,68..6,35. Dessin exactConnflylegacy type1 : corps2,5±0,38, post6,10±0,25, queue3,00, section0,64. Pas d’assimilation au DS1021-01 différent.

G=dessusNucleo→dessousshield, épaisseurshield1,6 déclarée, repèreX/Y constructeur converti rigidement commeV1.9. **Aucun changementXY/PCB/empreinte/schéma/routage/Gerber**,442fichiers historiques du shield identiques. Pas de nouvelERC/DRC ni testfirmware sur codeinchangé ; résultatsV1.9 réutilisés.

Calculsreproductibles : `I5=tipCN12+H5−G`, `C5=G−H5−bodyCN12`, `Iz=Hz+b+p−G`, `Cz=G−b−Hz`. Réserveplastique0,30 ; budgetG±0,10 et incertitudesCN12±0,10 explicitement **hypothétiques**. Intervalle admissible duGnominal **11,92..12,57mm sous ces bornes**, proposition12,25±0,10, aucune référence d’entretoise figée. À12,25 : **I5=4,15..4,81 / C5min1,01 ; Iz=4,00..5,72 / Czmin0,63mm**. À11,00 : jeuxnominalC5=0,09 etCz=−0,01, donc non robuste. L’intervalle ne vaut pas garantie de tous les montages réels ; mesuresCN12, assises/planéité, contacts et contreparties restent à qualifier.

**Entretoises** : bord du corpsZioSTEP et axe donnent0,285mm nominal pourØ4, plus que l’ancien0,04 facecourtyard. Mais pirecasbudgété−0,040 ; Ø3,5 donne0,535nominal/0,235 sous mêmes budgets. Le dessinSamtec autorise une bavuremax0,38 : le pirecasØ3,5 descend alorsà−0,145. **Pas deGO toutes tolérances**. Réductiondiamètre/formenonconductrice et contrôle des contreparties sont des pistes à qualifier, pas une substitutionBOM adoptée. Têtes≤4,5 horsfacesPCB, longueurvis/écrous/filetage/parois minces non libérés. Aucun trou déplacé.

## Assemblage et preuves réellement exécutées

`tools/build_assembly.py`, calculs `tolerances.py`, `sd_check.py`, `optical_probe.py`, `views.py`, `validate_study.py`, CadQuery2.8/OCCT/PyMuPDF. Deuxscénarios G12 puis12,25 étudiés, aucuncontactcaché déclaré acceptable par un testgéométrique.

- Aucune intersection volumique indésirable entre pièces sousshield testées et **sourceST complète**, àG12 et12,25, après filtrage spatial puis booléensOCCT. Post/socket sont des emboîtements intentionnels ; les corps/cavités sont des enveloppes, pas des ressorts de contact. DeuxvolumesMorpho du modèle ne correspondent pas à la population rapportée : affichage filtré et quatre mâles66/67/68/69 reconstitués selon mesures, **mais résultat d’intersection sourcecomplète conservé aussi**.
- Retenues analytiquesV1.5 inchangéesXY contre sockets/enveloppesPCBmodules : aucune intersectionpositive. **SD4682 réelle fabricant** placée avec transformationV1.5 contre deuxretenues : aucune. FRAM/MKI/Click et leurs composants partiellementprovisoires, révisionClick réelle inconnue ; pas de preuveglobale d’absencecollisions ni de résistancevibratoire/isolationGND2.
- STEPpropres éditables réouverts : **129solides et6solides**, valides ; PDF2pages rouvert/textes vérifiés, vueZ inspectée. Ces coupes sont des **enveloppes dimensionnées**, pas une section exacte de contacts internes. JSON et sourcesPython parsés. Normalisation whitespace des nouveauxSTEP sans changement de tokens.
- Troisrayons horizontaux candidatsversLED, au-dessus du modèleémissif et1mm au nord des origines, sont **obstrués par des objetsST**. Cela ne démontre pas l’invisibilité latérale oblique observée par l’utilisateur ni l’impossibilité d’un vitrage. Le cônecommun d’observation et la photométrie ne sont pas qualifiés. Aucun perçagePCB justifié par ces seulsrayons.

Fichiers et résultatsdétaillés dans `preuves/validation-finale.json`, `C05-collisions-G12p25.json`, `SD4682-contre-retenues.json`, `rayons-LED.json`, `tolerances.json`. Incidentdocumentairecorrigé : variable de liste des géométriesST écrasée par une liste de sockets dans le générateur ; métadonnées1060solides rétablies depuis la première inspection de la même source, scriptcorrigé, **résultatsbooléens inchangés**. Aucune règle ni test ancien affaibli.

AssemblagecompletST éditable conservé horsGit : `/home/lolo/dev/msm/reports/TR2-shield-v20-20261105/full-C05-G12p25-review.step`, hash dans `sources-locales.json`. Il comprend des enveloppes de modules, non de faux modèlesconstructeur complets. La SDexacte placée est fournie séparément. STEPtiers/propriétairesST et dessinSamtec ne sont pas redistribués intégralement ; scripts/STEPpropres/provenances versionnés. LecacheBREP local permet de reproduire sans répéter l’importSTEP original ; instruction de reconstruction dansSOURCES.

## Décision opérationnelle

**Pas de deuxième papier par automatisme.** Si les contreparties exactes sont déjà disponibles, un seul montageàblanc hors tension vérifierait ensemble emboîtement sans force, G/parallelisme, jeux auxplastiques/entretoises/soudures, accès et troisLED depuis une zonecommune. Aucune nouvelle mesure des8,22/2,40/15,2/10,2/0,4 demandée. Sans ces pièces, **aucune intervention demandée maintenant** ; ne pas acheter uniquement pour créer une pseudo-preuve.

Un premierarticle àrisque peut être examiné **après décisionhumaine distincte**, puis un seulshield assemblé/contrôlé àfroid avant alimentation. Ce serait une dépense avec risquedereprisePCB, **pas une qualification acquise ni commandeautorisée ici**. USER reste accèsdirectprotégé, RESETprotégé, vitragePC latéral ; CN15/STLINK/SD/RS485/nappe nécessitent encore les vraisfiches/efforts/rayons. Pas de défautdémontré imposant un changementPCB ; réservescoffret et gatephysique explicités, aucune classeIP ou robustesse industrielle revendiquée.

## Traçabilité

Sourceévaluée **0854d7c71ee3946ae7d1b2ced962cd87a9a7722e — Study V20 C05 3D mating, tolerance interval and unresolved physical gates**. Worktreeisolé`MSM_vibe-SHIELD-V20-EMBOITEMENT`, branchelocale`codex/shield-v20-emboitement`, ciblemain. HEADinitialce531d1e, basecontrat0ba0df16 danshistorique. Dépôtprincipalpropre82169af7, deuxstashes6251b988…/f3b829e6… et autresworktrees préservés. Journal `ETAT_COURANT_TR2.md` actualisé : XYconservé, Zconditionnel, tolérances/fixations/optique et suite. À transmettre àChatGPT : revue des bornes et de la décisionderisque, pas une libérationautomatique. Permissions exercées : étudeCAO/calculs/documentation/journal/commit/pushnormal seulement, aucunSTM32/COM/FRAM/SD/impression/achat.

Publicationnormale sur main **29c3d304c933e9d8ee76bb4297d431273bf82d1a** réussie, confirmée parfetch/ls-remote. **471fichiers relus intégralement depuisorigin/main et comparés octetpour octet**, livrables, preuves, historique du shield, contrat et gouvernance inclus ; preuve `REMOTE_READBACK.json` dansarchiveV20. Worktreepropre après ce premierpush. La présenteclôture et sonarchive sont publiées par derniercommit/pushnormal puis relues ; SHAfinal communiqué après vérification. **DONE signifie étude livrée, gatephysiqueBLOCKED et fabricationNO-GO maintenus, pas READY_TO_ORDER.**
