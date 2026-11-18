---
mission_id: TR2-20261117-SHIELD-DUAL-BRINGUP
revision: 2
status: BLOCKED
audit_status: DELIVERED_FOR_INDEPENDENT_REVIEW
base_ref: main
base_sha: f019dfbaa4e2cf9cf181031d0534013b89d989e1
initial_head: d5d3da95697607ad21cc57f95f48534e49ed3bb6
resume_head: 6b208d29591c4fc50af819e0bfe72d5c92d1f20f
result_sha: 6b208d29591c4fc50af819e0bfe72d5c92d1f20f
created_at_utc: 2026-11-17T21:35:26.068253+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# E-01 livré pour revue — alimentation des shields toujours NO-GO

**L’audit documentaire de la révision 2 est réalisé. La mission matérielle reste BLOCKED** sur le défaut SPI3 et les gates physiques. Ni le bring-up ni la libération électronique ne sont déclarés terminés. Le [rapport BLOCKED précédent](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/RAPPORT_AVANT_AVENANT_E01.md) est conservé intégralement.

## Résultats et livrables

- [Audit électrique E-01](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/AUDIT_E01.md) : sources, orientations, circuits, risques et réserves.
- [Matrice des 180 contacts](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/MATRICE_E01_180_CONTACTS.csv) et [synthèse des 17 nets](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/MATRICE_E01_17_NETS.csv), avec verdict individuel et NC.
- [RCA historique](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/RCA_SPI3.md), accompagnée de huit artefacts Git datés et hachés.
- [Inventaire achat/réemploi et options](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/REEMPLOI_ET_OPTIONS.md), sans supposer les connecteurs soudés récupérables.

La référence primaire est construite **sans lire l’ancienne matrice ni les fichiers KiCad** : netlist et PcbDoc ST C05, UM2861 pour les fonctions nominales multiplexées, Eagle Adafruit, dessin ST MKIGIBV2, schéma Click v102. Source/version/URL/SHA, fonction/direction/rail, schéma, PCB et deux cuivres CAM sont traçables. Cette lecture Codex ne remplace pas la seconde lecture indépendante de ChatGPT exigée par E-01.

**174 correspondances documentaires et 6 FAIL sur 180 contacts**, tous sur J8 :

| Contacts | Défaut |
|---|---|
|19 / CS et 20 / SCL|contacts requis laissés NC |
|21 / SDA|raccordé à ACC_CS au lieu de MOSI |
|22 / SDO|raccordé à ACC_SCK au lieu de MISO |
|23 et 24, NC constructeur|raccordés à MOSI et MISO |

La mesure utilisateur sur 001, TP3→SDO=0,3 Ω contre environ 600 kΩ vers SCL, corrobore le défaut physique d’horloge. Aucun relevé de ce segment sur 002 revendiqué. Les noms/types du symbole étaient eux aussi fondés sur la mauvaise affectation. Aucun PASS documentaire ne vaut continuité physique ou conformité d’un module réel.

## Contrôles réellement exécutés

Extraction fraîche du schéma par le CLI KiCad et lecture native du PCB, sans sauvegarde, remplissage de zones ou modification. Les 180 contacts des 19 empreintes sont recoupés avec leurs bibliothèques, retournements B.Cu inclus. Comparaison au PcbDoc ST de 106 positions : écart numérique maximal 1,024 nm, **sans prétention de précision de fabrication**. Recoupement des 18 contacts et 4 trous Adafruit. DIL24 MKIGIBV2 géométriquement correctement numéroté, affectation électrique incorrecte. Les 16 contacts du PcbDoc Click v101 concordent ; le natif v102 reste indisponible et n’est pas remplacé implicitement par v101.

Reconstruction géométrique depuis **tous les flashes, traits et régions des deux cuivres Gerber**, au-delà des seuls attributs X2 : 138 groupes après liaison des PTH et 4 vias, soit 17 nets et 121 îlots NC. Aucun court-circuit ou ouvert supplémentaire **par rapport aux nets du PCB** ; cela ne valide pas leur choix constructeur. Tous les perçages comparés aux Excellon. Approximation des cercles en 128 segments et coordonnées 1 µm documentées, sans mesure des PCB fabriqués.

- PCB V2.3 : SHA-256 `b79f2d6b1a0b44f2269087dcbdc6ba7c4ec9f5f41c704e4ebf77089f2d1729ab`.
- ZIP : neuf membres byte exact aux CAM et au manifeste ; SHA-256 `e91d5ce68ee4339b44ada627fd8fc5dc097b4e9b2a128b15611dd996a6e44d17`.

La revue V2.3 autorisait le téléversement, la préparation du devis et la soumission à l’utilisateur d’une commande de 5 prototypes ; elle précisait que le hash ZIP n’avait pas été recalculé indépendamment. **Le reçu/hash de l’upload effectif et les paramètres réellement fabriqués restent inconnus.** La fiche 90×100 mm / 2 couches / FR4 1,6 mm / 1 oz / HASL sans plomb n’en constitue pas une preuve.

Les scripts et sorties sont publiés dans E01. Les comparaisons ont été recalculées après enrichissement des fonctions multiplexées/auxiliaires, résultat inchangé 174/6. Aucun build firmware, ancien essai physique, test hôte ou ERC/DRC acquis n’a été rejoué.

Erreurs conservées : première lecture KiCad arrêtée sur `PCB_VIA::GetWidth called without a layer argument`, puis API corrigée avec couche explicite ; aucune assertion désactivée. Netlist ST lue ensuite en cp1252 après refus UTF8 ; cache de dépendances explicitement ajouté après import sexpdata absent. Téléchargement ST direct en timeout : dessin ST déjà obtenu via miroir identifié rendu et inspecté, source officielle consultée via outil documentaire. Click v102 retéléchargé avec même hash. Aucun de ces incidents n’est masqué comme un succès initial.

## Cause et autres interfaces

Première mauvaise affectation versionnée trouvée : **28 septembre, commit 74517bef**, avant sa reprise dans M015-K. La CAO du **2 novembre, 1d2aa668**, matérialise l’erreur dans le cuivre ; V1.9 conserve les nets en recalant les coordonnées, V2.3 conserve l’électrique en changeant la sérigraphie. Les contrôles ultérieurs vérifiaient une parité interne issue de cette même source erronée. Le décalage +2 est certain ; l’omission initiale des deux NC reste une hypothèse. Les acquisitions à fils réussies ne prouvent pas les numéros globaux réellement câblés. Aucun auteur intellectuel déduit d’un compte Git.

La RCA distingue les demandes de vérification des contrats/décisions des 2, 3 et 5 novembre, les contrôles géométriques Click v101, le schéma v102 cité, les parités internes et la qualification électrique indépendante manquante. Les messages non versionnés ne sont pas inventés.

**Aucune autre erreur d’affectation de pad trouvée dans les interfaces contrôlées**, avec réserves explicites : ancien libellé «Click PWM/DE» faux mais routage CS/3 correct ; TX MCU→13/TXD11 entrée, RX MCU←14/RXD8 sortie ; /RE via J3, pont J2 déclaré, J5/VCCSEL/J7/J8/TERM/BIAS réels non établis. Terminaison 330/165/110 Ω selon straps, pas 120 Ω supposés. GND2 reste isolé du shield. FRAM VIN/sortie LDO et rappels HOLD/WP/CS vérifiés ; SD SPI/NC vérifiés. Révisions/peuplement réels, contacts, tension VDDIO2 des GPIOG et marges/courant du 3V3 restent non qualifiés.

## Réemploi et suite

Conserver Nucleo, breakouts enfichés et médias. Une dépose de tous les connecteurs représenterait 164 contacts soudés par shield, avec risque pour pads et plastiques ; récupération non garantie. Quantités BOM et montage réellement constaté restent distingués, J5 potentiellement absent. La commande TME historique de 17,60 € TTC est rapportée comme telle, **sans réemploi de ce chiffre comme prix actuel ou valeur récupérable**. Stocks restants, prix de remplacement et délais non vérifiés : INCONNUS.

Première option à instruire après revue : reprise locale, deux liaisons à isoler et quatre à ajouter par PCB, sous réserve d’accessibilité et de maintien fiables. Aucun lieu de coupe qualifié : **ce n’est pas un mode opératoire**. Interposeur en repli avec impact de hauteur/contacts sur V2.7, puis nouveau PCB si reprise trop risquée, en réutilisant modules et stock compatible. Aucun choix, achat, dessoudage ou correctif physique exécuté. Aucune variante CAO n’était nécessaire pour établir ces preuves ; CAM historique inchangé.

**Aucune opération MCU, flash/debug/reset, SPI, COM, Modbus ou accès FRAM/SD dans E-01. Garder les deux shields hors tension.** Masse 002 encore à reprendre/contrôler ; FRAM 001 non qualifiée à 1 MHz et PASS 250 kHz borné distincts de SPI3. Acquis SD/FRAM préservés dans le rapport précédent. V2.3, V2.7, banc isolé et firmware inchangés contre 6b208d29 ; seuls errata documentaires ajoutés au vieux guide et à la matrice. Journal actualisé ; règles et mission inchangées.

À transmettre à ChatGPT : contre-lire les sources et les 180 chemins, décider la stratégie corrective, puis préparer les contrôles hors tension et seulement ensuite les qualifications conditionnelles. Ne pas rejouer les acquis inchangés. **La tranche d’audit est livrée ; l’alimentation et la libération restent NO-GO.**

## Git et publication

Worktree isolé `MSM_vibe-SHIELD-E01-AUDIT`, branche `codex/shield-e01-audit`, reprise depuis `6b208d29591c4fc50af819e0bfe72d5c92d1f20f`. Racine historique 82169af7, ancien worktree et deux stashes préservés ; aucun reset/clean/merge/rebase/force push. Commit/push exercés ; flash/debug autorisés mais inutilisés pour cet avenant.

Checkpoint `d485fe44ca98cee72bba55e27caa45a9cc9f89e4` publié et relu intégralement : 20 blobs comparés depuis GitHub. Le présent complément reçoit son SHA après commit/push normal sur `HEAD:refs/heads/main` et relecture intégrale ; SHA final et état Git communiqués dans la réponse finale. BLOCKED porte sur la correction/revue matérielle, pas sur une attente de publication. Aucune qualification indépendante revendiquée.
