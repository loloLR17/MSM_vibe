# V2.5 — fixations KISS, révision ciblée

**Coffret : M3×12 métalliques standard, une seule longueur. D126 : nylon Ajile exclusivement, déjà commandé. FRAM/SD et étriers : réserves localisées, pas de faux kit complet.**

Le ZIP `pack_imprimeur/TR2_V25_KISS_PARTIEL_REVUE.zip` est le seul candidat de cette révision : **14 types, 40 pièces pour deux coffrets**, réserves comprises. Il reste **partiel et destiné à la revue**, pas à transmettre comme lot complet sans décision humaine distincte, conformément à la revue indépendante V2.4. Les appuis FRAM/SD ne sont plus présentés comme libérés : leurs rondelles seules ne constituent pas un appui de compression indépendant des connecteurs. Les quatre étriers MKI/Click restent également exclus.

## Différences strictement nécessaires par rapport à V2.4

- Les 8 vis nylon M2×20 et 16 écrous Ajile sont **uniquement** pour les six D126, avec 2 vis/4 écrous de réserve. Aucun complément de nylon prescrit.
- Les 17 fixations de coffret par prototype passent à **M3×12 métalliques DIN912/ISO4762**, avec écrous M3 DIN934, AF5,5, hauteur≤2,4. Pour deux coffrets : 34 vis/34 écrous utiles, cible40/40 avec réserve. Utiliser stock ou assortiment courant ; aucun vendeur, achat, prix ou stock personnel supposé. Les 17 points ont chacun une fonction conservée : 4 couvercle, 3 bras Nucleo, 2 vitre, 2 bride nappe, 2 bride câble, 2 trappe outil, 1 USER, 1 SD. Aucun retrait arbitraire de point utile ni nouvelle famille de clips.
- Bride nappe rapprochée de la cassette de **2,5 mm** : sa forme imprimée est inchangée, seule sa position d’assemblage change. Le passage au M3×12 conserve l’engagement complet de l’écrou.
- Base : reprise locale de la poche M3 au coin(13;141), partiellement rebouchée par un rail ; deux petites fenêtres de chargement côté ouest, sous les niveaux des cartes, et prolongement des deux canaux de chargement des écrous de couvercle côté droit. Retouches motivées par des collisions calculées et conservées dans `preuves/rejet-*.json` ; **pas de modification du fond, G, trous du PCB ni D126**. Charger les neuf écrous captifs dans la caisse nue avant électronique/panneaux.
- **13 STL et 12 STEP individuels strictement identiques à V2.4**, couvercle ARNACO/cassettes/trappes compris. Seul le STL de base change ; deux STEP individuels changent (base et position de bride). Aucun nouveau logo à préparer. Original V2.4 et tous les dossiers antérieurs conservés.

## Appuis des modules : solution plus simple, mais non libérée

Solution retenue pour la prochaine vérification : **un appui PETG à la hauteur naturelle H, une vis M2 métallique ordinaire et un écrou par point**, avec portées/rondelles isolantes adaptées. Deux points par module, huit pour les deux prototypes. L’appui transmet directement la compression du module au shield ; la vis assure la retenue positive. Ne pas transformer la connexion en ressort ou utiliser le serrage pour tirer les broches.

Le montage V2.4 de deux écrous suspendus sous chaque module n’apportait pas ce chemin de compression vers le shield : le premier écrou se trouve à20,25mm, soit6,4mm au-dessus du shield. Une translation vers le bas éloigne la tête de vis de la face inférieure du shield ; l’absence de collision ne prouve donc pas un appui mécanique. Aucune qualification physique de ce montage n’avait été effectuée. Cette limite est explicitement corrigée dans la présente décision, sans réécrire les preuves V2.4.

Les fichiers Eagle Adafruit épinglés montrent quatre trous **métallisés Ø2,5, pastilles Ø3,2 non reliées à un signal** et des restrictions cuivre locales. C’est un fait du modèle constructeur, pas une identification des modules réels ni une preuve complète d’absence de cuivre/composants dans tout le volume de la visserie. Il n’y a pas d’interdiction générale du métal ; il manque encore **H réel**, le choix de longueur et le contrôle final des volumes/contacts sur les révisions réelles. `sources/appui_conditionnel.py` exige une hauteur et une référence de mesure explicites, sans défaut11mm, et écrit toujours hors ZIP avec statut non libéré. **Aucun STL d’entretoise de hauteur inventée ajouté au paquet.** Aucun achat de M2 supplémentaire recommandé à ce stade.

Les étriers V2.4 et leurs axes restent la base de reprise. Les disques de contact candidats débordent même des enveloppes rectangulaires provisoires des modules ; ils ne démontrent pas une zone nue. Pas de déplacement arbitraire des contacts vers un composant supposé libre. Les fichiers de référence restent en V2.4, hors ZIP ; confirmer zones nues et révision Click avant libération.

## Métal standard, preuves et limites

Référence dimensionnelle : [SFS/FERRONORM ISO4762 M3×12](https://www.sfs.ch/CH/de/dl/vlp/P_S010101_0185) : tête5,5×3, empreinte hexagonale2,5, filetage complet. [Böllhoff DIN934](https://eshop.boellhoff.de/out/media/pdf/DIN_934_Stahl_8_gv___en.pdf) : écrou M3 AF≤5,5, H≤2,4. Ces liens servent aux dimensions, **pas à proposer ces offres ni leurs prix**. Le contrôle emploie une enveloppe de tête plus large Ø6/H3,2 et des longueurs11,5/12/12,5mm ; cette plage est une marge de conception, pas une affirmation des tolérances de toute norme ou de tout assortiment. Les poches existantes AF5,9/H2,8 laissent0,2mm par face et0,4mm axial ; leur accessibilité est également testée, pas seulement leurs cotes.

Le métal structurel est confronté aux volumes complets des PCB/composants modélisés et aux1060 boîtes ST C05 : une séparation de ces volumes plus grands que le cuivre exclut le contact cuivre dans ce scénario. Aucune tige, tête ou écrou ne porte sur une carte ni ne relie le domaine logique et le bus RS485 isolé. La vraie isolation électrique, la tenue vibratoire, le fluage PETG et les variantes physiques restent à qualifier. Les bras Nucleo **imprimés** gardent leur patin isolant et le jeu V2.4 ; seule leur vis de pivot devient métallique, hors carte.

Les géométries et contrôles V2.4 inchangés (D126, ARNACO, fenêtres, services, maillages) sont réutilisés **par identité des fichiers**. La base ne perd que de la matière : sa propriété de sous-ensemble est testée, donc elle ne peut ajouter une collision C05 par ces retouches. Contrôles nouveaux : base/maillage modifiés, translation de bride, quincaillerie M3 aux trois longueurs, poches et chemin continu d’introduction des neuf écrous captifs. Pas de nouveau build/flash/debug/essai physique/ERC/DRC.

## Sources et reproduction

`sources/revision.py` lit V2.4 sans le modifier et dérive le dossier courant ; `controle.py` vérifie les exports et la quincaillerie, `controle_bras.py` le passage des pivots dans les bras PETG ; `appui_conditionnel.py` prépare seulement un appui futur explicitement non libéré ; `vues.py` génère les schémas ; `paquet.py` crée et relit le ZIP et vérifie la conservation.

Depuis ce dossier :

```sh
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/revision.py
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/controle.py
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/controle_bras.py
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/vues.py
python3 sources/paquet.py
```

Les sources nécessitent le dossier V2.4 versionné, pas un ZIP historique ni les modèles ST tiers. `cad/assemblage-imprimable.step` contient nos pièces ; `quincaillerie-M3x12-enveloppes.step` est un gabarit propre, pas un modèle fabricant ni une représentation exacte des filets. Premier article hors tension et décision indépendante avant usage. Aucun envoi à l’ami, achat ou impression effectué.
