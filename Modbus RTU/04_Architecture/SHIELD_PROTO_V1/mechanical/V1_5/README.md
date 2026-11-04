# Intégration V1.5 — assemblage ouvrable, contrôle physique nécessaire

**READY_FOR_PHYSICAL_CHECK / BLOCKED_FABRICATION. NE PAS COMMANDER NI IMPRIMER POUR CAMPAGNE.** Le PCB reste le candidat électrique V1.4 inchangé. Cette maquette assemble ses pièces et prépare un boîtier ; elle ne constitue pas une géométrie C05 qualifiée.

## Ouvrir et modifier

- `REVIEW_ONLY/assembly-review.step` : assemblage STEP avec noms de pièces, Nucleo reconstruite, shield perforé, J1–J9 et leurs contreparties nominales, FRAM, SD, MKI, Click, retenues, entretoises, boîtier et réservations fonctionnelles. Le modèle SD provient du fabricant ; les autres géométries sont distinguées dans `parameters.json` et `docs/checks/v15/assembly-parts.json`.
- `parameters.json` et `../../tools/build_integration_v15.py` : sources paramétriques éditables (CadQuery 2.8 / OCCT). **X CAD = X KiCad ; Y CAD = −Y KiCad ; Z=0 au dessus shield.** Les boîtes XY du JSON sont en vue KiCad. Tous les centres C05 inconnus sont `null` ; aucun placement arbitraire ne les remplace. Les dimensions de détail du coupon et des inserts sont des constantes éditables dans le script ; le JSON ne constitue pas une interface de réglage automatique de tous ces détails.
- `REVIEW_ONLY/assembly-views-and-sections.pdf` : huit vues ; PNG séparés. `section-X60.step` et `section-Y92.step` sont des demi-assemblages obtenus par intersection BRep exacte, non des plans de fabrication cotés.
- Chaque pièce du boîtier est exportée séparément en STEP et STL. `Case_base_PETG_CONCEPT` et `Case_lid_PETG_CONCEPT` restent des **propositions non libérées**, pas un kit d’impression fonctionnel. Les panneaux LED/USER et inserts de câble sont volontairement aveugles.
- `operator-principle-NOT-C05.step/pdf/png` et `principle-*.stl` : mécanisme captif et trois guides standards sur **coupon séparé**, dont l’origine est fictive. Ce n’est pas leur implantation dans le TR2. Le porte-guide du coupon dispose de montants et d’une traverse vissée pour fermer le chemin de charge vers son panneau. Ressort/diaphragme de rappel et embout compliant restent à dimensionner ; aucune garantie d’effort admissible.
- `CONTROLE_PHYSIQUE_MINIMAL_2P.pdf` : liste de contrôle future, deux pages. Ce document ne demande aucune intervention pendant la mission V1.5 et ne remplace pas le gabarit V1_4.

## Reproduire sans accès matériel

Dans un environnement Python dédié avec `cadquery==2.8.0`, `matplotlib==3.11.2` et `trimesh==5.2.0` :

```sh
python tools/build_integration_v15.py
python tools/review_integration_v15.py
```

Commandes à lancer depuis le dossier SHIELD_PROTO_V1. Elles n’accèdent ni à un debugger, ni à COM, ni aux supports physiques. L’environnement effectivement utilisé est dans le cache MSM ; aucune installation système ou modification Windows n’a été faite. Les versions complètes sont consignées dans les preuves.

La vérification de protection utilise le commit initial V1.5 `10c6e0f22cd09f203ab79aedd79e7e5ebda3637f`, qui doit être présent dans l’historique. La génération autorise des chevauchements d’assemblage voulus (pins/sockets, appuis) ; elle **ne prétend pas effectuer un audit exhaustif de collision entre toutes les pièces réelles**.

## Provenance et limites

Les contours/perçages du shield sont pris dans le PCB sauvegardé, épaisseur déclarée 1,6 mm. Son ancien STEP « board-only » présente un cœur de 1,51 mm (observation OCCT) ; il n’est pas silencieusement assimilé à une enveloppe d’assemblage de 1,6 mm. Le PCB/générateur électrique n’est pas modifié pour cette différence de représentation.

Adafruit4682 : STEP au commit `6c5cd9e3e65252bb5d5339608cf8d3cdabcef251`, fichier original dans `models/`, SHA-256 `195927fa6892b80caf19cd01887e84f1202026665b393f9c8c9f399fa6b4d6e8`. Transformée rigide +90° Z puis (43,04 ;−108,54 ;11) : les neuf pads et les deux trous sont comparés à la géométrie KiCad, sans déformation ni miroir du solide. Attribution Adafruit Industries, CC BY-SA 3.0 ; licence fournie à la racine du dossier shield. Modèle nominal fabricant, pas une mesure du breakout détenu.

Click : PCB/enveloppes paramétriques fondés sur les sources v101 antérieures ; le STEP v101 a été inspecté pour ses bornes, mais le modèle exporté n’est pas une reproduction détaillée de ses 163 solides. La révision de l’exemplaire utilisateur est inconnue. MKIGIBV2 : contour et hauteur de composants explicitement provisoires ; seule la connexion DIL et l’entraxe acquis sont connus. Nucleo : reconstruction UM2861, **aucun CAD de fabrication C05 obtenu**. Les J1–J9 sont des enveloppes de catalogue, pas des contacts détaillés qualifiés.

Les trois LED réellement utilisées sont LD1 verte/PC7, LD2 bleue/PB7 et LD3 rouge/PG2 ; USER est B1/PC13. Ces identités viennent du code prototype conservé et de ST ; elles ne donnent pas leurs centres mécaniques. LD4/5/7 ne sont pas les trois indicateurs d’acquisition.
