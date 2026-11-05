# Revue indépendante ChatGPT — V2.5 KISS, fixations et dossier d'impression

Date : 2026-11-05. Dépôt `loloLR17/MSM_vibe`, branche `main`. HEAD de référence contrôlé : `6b5447854dc76f0695171cdf4ff9bca0e2ea1f68`.

## Contrat, rapport et publication

Mission `TR2-20261105-SHIELD-V25-KISS-FIXATIONS`, rapport du même identifiant `status: DONE`, commit technique `ff97bb313dc3b9cb191f08a84c479f5015b01962`, commit de clôture `6b5447854dc76f0695171cdf4ff9bca0e2ea1f68`. Relation parent/enfant vérifiée via l'API GitHub : le commit technique est enfant de `72b47ee78c03b09d4a3d6d9b8f69eeb8258d7c35` et parent direct de la clôture. Le contrat reste `READY`, mais ne doit pas être rejoué avec un rapport `DONE` du même identifiant.

## Verdict

**V2.5 acceptée comme correction géométrique/documentaire KISS, avec qualification numérique ciblée. Le ZIP demeure PARTIEL pour revue, pas un dossier complet prêt à envoyer une seule fois à l'ami imprimeur.**

La mission conserve sans modification la visserie nylon Ajile `8 × M2×20 +16 écrous M2` pour le seul empilage D126, dont 6 vis/12 écrous utiles et 2/4 en réserve. La fixation structurelle du coffret utilise désormais une **unique longueur de vis métallique ordinaire M3×12 à tête cylindrique six pans creux DIN912/ISO4762**, et des écrous hexagonaux standards M3 DIN934 (AF5,5, hauteur maximale cible2,4 mm) : **17 fixations/boîtier, soit34 vis et34 écrous utiles pour deux prototypes**,40 de chaque en incluant6 pièces de réserve. Aucune nouvelle commande de nylon spécialisée demandée ni exécutée.

**Le sens précis de KISS ici est la réduction des références de visserie et l'accessibilité des écrous, pas la suppression arbitraire des17 fixations par boîtier.** Les 17 fonctions sont documentées ; aucune économie de coût global n'est encore prouvée.

## Contrôles directs et déclarations distinctes

- ChatGPT a directement relu depuis `main` les fichiers `V2_5_KISS_FIXATIONS/README.md`, `BOM_QUINCAILLERIE.csv`, `QUANTITES.csv`, `IMPRIMEUR.md`, `RESERVES.md`, `preuves/M3-controle.json`, `reprise-geometrique.json`, `appuis-et-limites.json`, `etriers-reserve-localisee.json`, `appui-generateur-controle.json`, `pack-manifeste.json`, `conservation-finale.json`, les logs et commits publiés.
- Comptage indépendant à partir du CSV `QUANTITES.csv` : **14 types, 40 pièces**, dont huit D126 (six utiles et deux de réserve), deux bases et deux couvercles ARNACO. `BOM_QUINCAILLERIE.csv` confirme40 M3×12 et40 écrous M3 cibles, sans imputer la visserie Ajile aux modules ou à la caisse.
- Relecture de `preuves/M3-controle.json` et du journal final : **17 fixations par kit**, trois longueurs simulées11,5/12/12,5 mm, enveloppe de tête Ø6/H3,2, écrous AF5,5/H≤2,4, poches AF5,9/H2,8, `positive_intersections: []`, `PCB_copper_contact: false`, `passed: true`. C'est une **preuve de contrôle exécuté par Codex et rapporté dans le dépôt**, pas une réexécution indépendante de tous les booléens CadQuery ni une certification diélectrique. Le fichier `rejet-logements-M3.json` contient les intersections du scénario initial rejeté ; il n'est pas le résultat final et le scénario final est corrigé.
- `preuves/reprise-geometrique.json` rapporte 13 STL identiques à V2.4, nouvelle base ayant **seulement 163,200856 mm³ retirés**, déplacement en assemblage de la bride nappe de2,5 mm, D126 et marquage ARNACO inchangés. Le contrôle d'identité binaire de 683 fichiers historiques et des STL/STEP est déclaré dans les preuves de Codex ; pas de relecture octet par octet indépendante de l'ensemble par ChatGPT.
- ZIP `pack_imprimeur/TR2_V25_KISS_PARTIEL_REVUE.zip` déclaré **21 entrées,14 STL,40 pièces**, SHA256 documentaire `bd1b6bee3cc7566d5464921eaef156adff31121b734342e0339d995c3cc42479`. L'ouverture réelle du ZIP binaire et son hash n'ont pas été reproduits indépendamment par ChatGPT ; les contrôles complets CRC et empreinte sont déclarés par Codex. Recalculer avant tout transfert externe.

## Deux blocages locaux résiduels : ne pas les masquer

1. **Appuis FRAM et microSD (huit points pour deux shields)** : V2.4 suspendait chaque module à deux écrous sous la carte, mais **sans chemin de compression prouvé jusqu'au shield**. `appuis-et-limites.json` décrit un vide de6,4 mm entre écrou bas et dessus shield. V2.5 refuse à juste titre cette solution : adopter en principe un appui isolant porteur PETG de hauteur réelle H, une vis et un écrou M2 ordinaires avec portées isolantes. **Hauteur naturelle H et compatibilité des vraies révisions restent inconnues**, donc aucune cote arbitraire, STL libéré ni achat M2 supplémentaire à ce stade. Les trous Adafruit documentaires sont métallisés, non connectés à un signal, pas des NPTH garantis sur les cartes effectivement reçues.
2. **Deux étriers MKIGIBV2 et deux étriers Click RS485** : quatre modèles hors ZIP. Les points de contact candidats débordent des enveloppes PCB provisoires d'environ0,56–0,61 mm ; cela ne valide pas la présence de bord libre ni l'absence de composants/IDC. Vérifier les modules réels avant de déclarer ces pièces imprimables. Ne pas faire porter une contrainte sur un composant ni perdre l'isolation RS485.

## Décision de pilotage

**GO pour conserver V2.5 comme base CAO KISS vérifiée ; NO-GO pour appeler ce ZIP un ensemble d'impression complet et le transmettre à l'imprimeur comme tel.** Pas de commande de nylon spécialisé, pas de reprise générale du boîtier. Les besoins utilisateur sont un seul dossier complet pour deux prototypes, imprimable sans travail de conception par son ami.

La prochaine levée de réserves est strictement ciblée : déterminer la hauteur H d'emboîtement libre et identifier les surfaces porteuses nues des vrais breakouts MKI/Click ; un seul contrôle à froid quand matériel présent, sans répéter les cotes déjà acquises. Puis finaliser seulement supports/étriers et mettre à jour le ZIP avec les quantités. Première mise sous tension uniquement après inspection physique, sans forcer les connecteurs ni créer de contact métallique entre domaines isolés. La fenêtre sera taillée dans de la récupération, les garnitures dans de la chambre à air/feuille de joint, les colliers nylon courants. Aucun contrôle physique, achat, impression ou envoi n'a été exécuté pendant la présente revue.
