# V1.3 — synthèse de précommande

Mission TR2-20261103-SHIELD-V13-J5-FEMELLE-BOM. **NE PAS COMMANDER / BLOCKED_FABRICATION.**

| État | Résultat |
|---|---|
| CAD_VERIFIED | Contrôles électriques/CAM du candidat, distincts de la compatibilité mécaniqueC05 |
| READY_FOR_PHYSICAL_TEMPLATE_CHECK | Gabarit1:1 et planJ5 femelle prêts pour une suite physique autorisée |
| BLOCKED_FABRICATION | Dimensions mâlesJ5/empilageZio et géométrieC05 non démontrées ; enveloppesMKI/Click à confronter aux réservations |

L'observation opérateur «mâles69/70 présents sur les deuxC05» remplace l'hypothèse de Nucleo non équipées. **Aucune modification de Nucleo à prévoir.** L'item3 initial des décisionsprécommande et les rapportsV1.2 parlent encore de pads libres/femelles à ajouter : l'avenant opérateur et le contratV1.3 prévalent explicitement ; documents humains et archives ne sont pas réécrits par Codex.

CAO effective : J5 devient `J5_CN12_69_70_PG4_FEMALE`, B.Fab avec corps/entrées, B.CrtYd et sérigraphie miroir de repérage ; pads1/2/centres/nets inchangés. Générateurs et schéma incluent la femelle. J1–4 et J6–9 gardent leurs positions/pads ; seules enveloppes sockets/headers sont adaptées aux références standard. Les32trajets cuivre et zones fonctionnelles restent ceux deV1.2, aucun reroutage. C1/C2 restent ceux validésV1.2. Boîtier90×100 et tous perçages/ancrages conservés. Voir `J5_ET_EMPILEMENT_V1_3.md`, `CONNECTEURS_STANDARD_V1_3.md`, BOM/budget.

SourcesST : nouvelle tentative bornée HTTPS sur les deux ZIP officiels,8,4s chacun, timeout et aucune archive reçue ; logs `checks/v13/st-official-bounded-retry.json`. Les échecs navigateurV1.2 sont déjà conservés, pas de nouvelle campagne réseau identique. Révision interneC05/C02/C03/format/hash demeurent inconnus ; publication4.0/3.4 n'est pas une révision de PCB. Gabarit issu du PCB sauvegardé et de la reconstructionUM2861Rev4, jamais présenté comme extractionC05.

**Dernier contrôle à regrouper en une intervention ultérieure quand l'utilisateur sera disponible**, sans action demandée maintenant :

1. DeuxC05 hors tension, gabarit `gabarit-C05-V1_3.pdf` pages1/2 à100%, barresX/Y100mm contrôlées : tous centresZio, paireCN12-69/70, trois ancrages et contour. Page3 = miroir de contrôle seulement.
2. Même inspection : vues de chaque paire mâle69/70 dessus/profil avec règle, section/hauteurcorps/longueurexposée ; hauteur naturelle d'emboîtement Zio, et dégagement entretoises/vis/queues sur les deuxfaces. La seule présence du mâle ne qualifie pas la femelleH8,5. Pas d'insertion forcée, dessoudageNucleo ni rattrapage par serrage.
3. Vue contoursMKI/Click réels/révision pour vérifier les réservations d'ancrages et l'isolation ; pas de répétition de l'entraxe15,2 ou GNDMKI déjà acquis.

Si ces cotes imposent une autre hauteur ou un autre corpsJ5, corriger CAO et contrôles avant Gerber libéré. Les dimensions de clip/poussoir/LED/boîtier final et détrompeurs automatiques ne sont pas des gates PCB nus ; les retenues positives et réservationsd'accès sont conservées. Aucune qualification physique/électrique/vibratoire déduite des exports.
