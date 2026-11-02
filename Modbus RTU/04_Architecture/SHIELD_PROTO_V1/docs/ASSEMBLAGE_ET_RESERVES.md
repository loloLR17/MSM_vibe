# V1.1 — assemblage et gates de libération

**BLOCKED_FABRICATION : ni PCB ni retenue imprimée autorisés pour campagne.** Sources V1 conservées au commit1d2aa668 ; la clôture ChatGPT n’a pas levé leurs réserves. La V1.1 remplace la simple proposition de sangles par des étriers paramétriques et précise les interfaces opérateur ; elle ne constitue pas un assemblage réalisé.

## Séquence future, sans manipulation demandée maintenant

1. Contrôler gabarit1:1 et cotes C05 ; figer hauteurs de sockets/entretoises, engagement et dégagement de soudure. Vérifier les trois ancrages non alignés (32,62;21,19), (33,89;97,39), (79,61;97,39), perçage2,8 pourM2,5. Le quatrième emplacement proche CN7 reste omis pour éviter une tête de vis dans le connecteur.
2. Souder headers J1–J5 sous le shield, monter à blanc sans pression latérale ni flexion ; les entretoises portent l’effort. Vérifier la paire J5 en particulier. Déposer hors tension pour contrôle de continuité/isolation et nettoyage.
3. Poser les sockets J6–J9 dessus ; queues sous les modules propres/courtes, pas de tête métallique ou carte voisine en contact avec un conducteur. Les courtyards sont ceux des sockets au niveau du support, les enveloppes élevées sont F.Fab. Hauteur11mm = cible, non mesure.
4. Visser FRAM et SD dans leurs deux trous existants avec entretoises isolantes. Fixer d’abord l’ancrage Nucleo caché sous SD. Ne pas utiliser le serrage pour rattraper un défaut d’alignement ; appui sur butée de hauteur avant serrage. La SD sort vers-X, sans démonter le module.
5. Adapter puis installer les étriers J8/J9 aux surfaces de rives et clés réellement vérifiées. La dissymétrie des vis ne remplace pas le détrompage du module. Contrôler leur démontabilité et le dégagement des composants/nappe/borniers. Ne pas imprimer les gabarits `REVIEW_ONLY` comme pièces de campagne sans cette revue.
6. Installer les selles nappe/RS485/USB sur la structure du futur boîtier, jamais sur les connecteurs. Prévoir boucle de mou, arêtes arrondies, protection de la SD, fenêtre LED et USER captif avec butée ; RESET séparé et protégé. Aucun boîtier complet n’est défini ici.
7. Après validation mécanique, exécuter le plan électrique/physique sous une mission dédiée autorisée ; arrêter avant de compromettre FRAM, SD et firmware existants.

## Réserves minimales restantes

| Élément extérieur nécessaire | Relevé minimum utile lors d’une intervention ultérieure autorisée | Ce qu’il permettra de figer |
|---|---|---|
| Nucleo C05 | Gabarit confronté ou CAO C05 ; vue dessus cotée des ports, LED, USER/RESET et ancrages ; hauteur engagée des connecteurs | Alignement sans flexion, standoffs, ouvertures/guides et longueur/course USER |
| MKIGIBV2 | Longueur/largeur hors tout, hauteur et position/débord du connecteur, vue dessus/profil et rives sans composants | Enveloppe, patins et détrompage réellement asymétrique du module |
| Click réel | Révision lisible et vue des borniers/rives ; vérifier concordance v101/v102 et hauteur | Contacts de l’étrier, butée de renversement et accès fils/outil sans pont d’isolation |
| SD assemblée | Hauteur/retrait exact de la bouche et engagement dans les sockets choisis | Poche doigts, trappe et trajectoire de carte ; comparer aux positions0,6/1,6/5,3 de la fiche |
| Câbles | Épaisseur nappe/rayon admissible ; diamètres et coques USB/RS485 utilisés | Gorges/selles, boucles de mou et ouvertures sans pincement |

**Ne pas redemander** l’entraxe MKIGIBV2 déjà mesuré15,2mm ni la position GND déjà rapportée en bas à droite deJP2 (global13). Les centres LED/USER ne sont pas déduits avec une fausse précision d’une photo. Ces inconnues sont matérialisées dans `mechanical/interfaces.json`.

Même après ces cotes, le budget livré, la puissance disponible3V3 sous pics SD/Click, l’insertion sans flexion, l’accès opérateur, la tenue thermique/vibratoire et le transport devront être vérifiés. Le modèle v101 et la présence d’une zone hors cuivre ne certifient pas la tenue diélectrique du montage réel. Pas de fabrication libérée par un ERC/DRC vert.

SPI3 reste le pinout actuel, sans modification de fréquence, du filtre ou des formats. Les30cm de nappe et8MHz restent à qualifier après montage ; ne pas supprimer les résistances série existantes. Carter peint56,5°C, fixation de tête et antichute restent un chantier distinct.
