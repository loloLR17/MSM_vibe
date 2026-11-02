---
mission_id: TR2-20261102-SHIELD-PROTO-V1
status: IN_PROGRESS
base_ref: main
base_sha: 4d45c9fdbc0031cc6df10a0b1bea1515e94b3b3c
initial_head: ad924a3d88ee9cd0908823aa333fec183c114dd3
result_sha: null
created_at_utc: 2026-11-02T21:26:17Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield prototype V1 — CAO livrée, fabrication bloquée

Le travail logiciel CAO est préparé pour publication. Ce premier statut `IN_PROGRESS` couvre la publication/relecture distante encore à effectuer ; **la libération fabrication reste BLOCKED** pour les raisons mécaniques ci-dessous. Aucun matériel n’a été sollicité.

## Livrables réalisés

Dossier [SHIELD_PROTO_V1](../../04_Architecture/SHIELD_PROTO_V1/README.md) : véritable projet KiCad10.0.6 éditable, schéma, PCB routé deux couches90×100mm, bibliothèques relatives locales, générateurs et vérificateurs.19empreintes,187pads dont11NPTH,59broches connectées et117îlots NC,32liaisons routées hors plans de masse,4vias. Pistes0,25mm, isolation minimale projet0,20mm, vias0,65/perçage0,30mm ; GND sur deux faces. Le cuivre et les connecteurs ne constituent pas une qualification physique ou d’impédance.

Exports réellement générés : [schéma PDF](../../04_Architecture/SHIELD_PROTO_V1/exports/schema.pdf), implantation PDF, dessous miroir, gabaritSVG1:1, STEP et image3D **support nu uniquement**, [Gerber X2/Excellon candidats](../../04_Architecture/SHIELD_PROTO_V1/exports/BLOCKED_FABRICATION/NE_PAS_COMMANDER.txt). Les Gerber sont un jeu réel exporté, **interdit de commande en l’état**. Pas de modèle complet Nucleo/modules prétendument contrôlé en3D.

[Matrice électrique](../../04_Architecture/SHIELD_PROTO_V1/docs/MATRICE_INTERFACES.md) : SPI3PC9–12 vers kitST ; **SPI2**PB13/PD3/PD4/PF12 vers Adafruit4682, sans SDMMC ; SPI1PA5–7/CS_PD14 vers FRAM4718 ; LPUARTPG7/PG8 et DE_PG4 vers MIKROE3863. Pas deGND2bus ou5V routé. Le Click exige VCCSEL3V3 et son pont DE-/RE déjà documenté. Variante candidate FRAM alimentée sur **VIN**, sortie régulateur3V3 laisséeNC, pour lever le libellé historique ambigu sans supposer le câblage effectif.

[Sourcing](../../04_Architecture/SHIELD_PROTO_V1/docs/SOURCES.md) : PCB Eagle Adafruit officiels figés par commits et hashes, mécanique Click v101 importée pour son connecteur, schéma électrique v102 identifié séparément, UM2861 Rev4 et tables Rev10. Téléchargements directs ST C05/MKIGIBV2 en timeout : aucune cote manquante inventée. Attribution/licence AdafruitCC-BY-SA3.0 jointe. La bibliothèque Morpho144 générique, incompatible sans adaptation, n’a pas été réutilisée comme preuve C05.

Observations utilisateur conservées séparément : NucleoC05 et références des modules ; entraxe MKIGIBV2 **15,2mm au pied à coulisse** (nominal proposé15,24), pas2,54mm rapporté ; GND à l’extrémité basse droite deJP2 vue dessus. Correspondance schéma global13, et non le numéro absolu12 ; l’utilisateur comptait les12positions. Aucune mesure de continuité ou manipulation demandée ici.

## Résultats de validation effectivement observés

Les sorties et arguments sont dans [docs/checks](../../04_Architecture/SHIELD_PROTO_V1/docs/checks/commands.json).

| Contrôle exécuté | Résultat |
|---|---|
| KiCad `sch erc --severity-all --exit-code-violations` | code0,0anomalie |
| KiCad `pcb drc --schematic-parity --all-track-errors --severity-all --exit-code-violations` | code0,0violation,0non-connecté,0écart de parité |
| Export PDF/netlist/Gerber/drill/STEP/rendu3D | codes0 réellement obtenus |
| `tools/verify_exports.py` sur netlist exportée et PCB relu par pcbnew | PASS :59broches, plus117NC identiques |
| Vérification CAM indépendante du générateur | PASS : attributs des pads connectés sur les2cuivres, tous centres/diamètres Excellon (NPTH/PTH/vias), contour90×100 identique |
| Inspection visuelle | schéma lisible, carte nue et implantation examinées ; empilage complet NON VÉRIFIÉ |
| Syntaxe Python des outils | compilation Python réussie, aucun test STM32 impliqué |

La vérification CAM ne reconstruit pas indépendamment toute la topologie cuivre ni l’intégrité du signal. La parité schéma/PCB et le DRC natifs contrôlent la connexion du PCB ; les comparaisons d’exports ne sont pas une certification de fabrication.

Erreurs de développement conservées dans [les preuves](preuves/TR2-20261102-SHIELD-PROTO-V1/LECTURE_PREUVES.md) : premier DRC39anomalies (dégagements, perçages, sérigraphie, enveloppes), tentatives de routage sans passage, puis117écarts de noms desîlots NC lors du contrôle de parité renforcé. Corrections : Click sur aile latérale, résistances dégagées, calcul d’obstacles géométrique et maillage0,1mm, dégagements conformes sans abaisser le seuil0,20mm, troisième fixation retenue au lieu d’une tête de vis en conflit, courtyards des sockets séparés des enveloppes élevéesF.Fab, GND solide et correction des libellés/sérigraphies ; reprise des noms NC exacts de la netlist. Aucun test désactivé ni erreur masquée pour obtenir le résultat final. Les catégories ignorées par défaut par KiCad restent visibles dans ses JSON ; aucune exclusion spécifique ajoutée.

`tr2_validate.sh` non exécuté : aucun firmware, cœur logiciel ou harness modifié. Les contrôles utiles sont ici ERC/DRC/parité/export et inspection du dossier ; une compilation STM32 n’apporterait pas de preuve sur les cotes du shield.

## Coût, critères non satisfaits et suite minimale

[BOM](../../04_Architecture/SHIELD_PROTO_V1/docs/BOM.csv) et [budget](../../04_Architecture/SHIELD_PROTO_V1/docs/BUDGET.md) pour2équipements avec réemploi intégral : **scénario45,11€**, comprenant lot5PCB, connecteurs, passifs, fixations/retentions, provision17€ de transport et réserveTVA. Certains prix de connecteurs sont observés ; passifs/fixations/port/fiscalité sont des provisions. Le prix PCB2USD est un appel commercial, non un devis livré. Convention budgétaire1USD→1EUR explicitement hypothétique, pas un cours de change. Socket12hors stock, variantes à vérifier ; références finales de passifs/fixations non figées. **Le coût réel livré≤50€ n’est pas démontré.**

**Obstacle réel à la fabrication :** CAO C05 indépendante inaccessible et enveloppe/hauteur MKIGIBV2 non obtenues ; sans ces éléments, le détrompage/retention et l’empilage sûrs ne peuvent être figés. Les sockets ouverts et une sangle ne satisfont pas, à eux seuls, le critère de détrompage. La portion mécanique reste préparée mais non qualifiée.

Les [réserves minimales](../../04_Architecture/SHIELD_PROTO_V1/docs/ASSEMBLAGE_ET_RESERVES.md) sont :

1. Confronter le gabarit à la NucleoC05 ou obtenir sa CAO, vérifier connecteurs/ancrages et accès.
2. Obtenir vues/cotes hors tout et hauteur/débord du MKIGIBV2 pour dimensionner l’étrier détrompé. Ne pas redemander l’entraxe ni la position GND déjà rapportés.
3. Vérifier l’empilage réel, les interfaces Click v101/v102, les hauteurs des sockets disponibles et la reprise d’effort ; figer passifs/fixations et coût livré, puis adapter CAO/exports avant revue de libération.

Aucune manipulation supplémentaire immédiate demandée : le contrat exclut les interventions physiques. Les observations pourront être obtenues lors d’une tranche ultérieure autorisée. [Plan de qualification](../../04_Architecture/SHIELD_PROTO_V1/docs/PLAN_QUALIFICATION.md) fourni : continuité/isolement/alimentation, capteur/FIFO, SD avec vérification intégrale, FRAM protégée, RS485 puis coexistence et répétabilité. **8MHz, vibrations, CEM, température et futur montage de tête à56,5°C restent non qualifiés.**

## Préservation, journal et Git

Worktree isolé `MSM_vibe-SHIELD-PROTO-V1`, branche de travail `codex/shield-proto-v1`, basé sur `ad924a3d`, dépôt d’origine inchangé. Les deux stashes6251b988 etf3b829e6 préservés. Aucun reset/rebase/merge/clean/force push. Seuls dossier CAO, preuves/archives de cette mission, dernier rapport et append factuel du journal sont inclus.

Journal `ETAT_COURANT_TR2.md` actualisé : CAO/contrôles acquis, confirmations utilisateur, fabrication bloquée et prochaine étape. Les décisions d’architecture gelées sont inchangées. FirmwareCN0, archivesCN0/GI0/C29/OBE, FRAM, deux microSD et deux cibles préservés. RécupérationTR2-002 toujours en attente ; écrêtage et fixation tête restent des sujets distincts. Aucun flash/debug/COM/accès média, achat ou changement Windows exécuté.

Publication et relecture distante à consigner après le premier push. Le SHA du commit portant le rapport sera indiqué séparément après publication, sans tentative de référence circulaire.
