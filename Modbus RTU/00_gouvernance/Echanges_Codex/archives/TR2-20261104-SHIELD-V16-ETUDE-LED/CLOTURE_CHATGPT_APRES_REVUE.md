# Clôture administrative V1.6 — suite vitrage latéral et USER

Date : 2026-11-04. Dépôt `loloLR17/MSM_vibe`, branche `main`.

Mission `TR2-20261104-SHIELD-V16-ETUDE-LED` : rapport Codex `status: DONE` **pour l'étude seulement** ; source évaluée `a526c1c162aa365c9701a2bcbc3f1c0bcddabcf6` ; rapport final `ecfc0ddc2539d7f5b98cadea1e46bc8449aefc48`. Revue indépendante ChatGPT publiée au commit `25a1fdd36463e8c0c5bf1d6881f9e167afdea4f4`. Aucune modification du PCB, aucun feu vert fabrication.

**Nouvelle observation utilisateur sur TR2-001 (04/11/2026) :** les trois voyants d'état Nucleo LD1/LD2/LD3 apparaissent **visibles quasiment perpendiculairement au plan de la carte depuis le côté ST-LINK**, dans un essai réel sans shield/boîtier final. L'utilisateur suggère une **ouverture vitrée dans la paroi côté ST-LINK**, au lieu d'une fenêtre de couvercle à visée oblique. Cette observation est un élément réel précieux, **pas encore une preuve de ligne de vue dans le coffret équipé du shield, du MKIGIBV2, de la nappe et des retenues**. Priorité à étudier cette option de vitrage latéral simple, en évitant une modification électrique si possible. Les guides sophistiqués ne sont plus imposés.

**USER** : hauteur au repos du sommet bleu mesurée utilisateur à **10,2 mm au-dessus du PCB Nucleo**. L'utilisateur demande s'il faut aussi mesurer la **course** pour préparer le poussoir captif sans autre aller-retour. Cette course, l'effort admissible, la position et la hauteur de l'appui restent à qualifier sans forcer le bouton : le relevé de course devra distinguer « premier déclenchement » de « butée mécanique » ; ne jamais suggérer d'écraser le contact au pied à coulisse. Le choix de l'actionneur extérieur ne doit pas reposer sur une course fictive.

La présente clôture est administrative, sans altération des rapports archivés. La prochaine mission est une **vérification ciblée de l'option vitrage latéral et du poussoir USER**, pas une refonte ni une commande. Les autres gates C05/J5/Zio/Click/MKI demeurent.
