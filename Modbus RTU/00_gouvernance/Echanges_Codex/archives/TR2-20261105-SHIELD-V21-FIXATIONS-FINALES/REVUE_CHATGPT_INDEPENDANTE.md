# Revue indépendante ChatGPT — V2.1 fixations finales (05/11/2026)

Dépôt `loloLR17/MSM_vibe`, branche `main`. Mission `TR2-20261105-SHIELD-V21-FIXATIONS-FINALES`, rapport Codex `status: DONE` pour étude CAO seulement ; réalisation `ea1ce5507d9386d7dfa4014d1c26162bb539c280`, clôture `b38a242d5e15a743a8d606470bdfb6dd54bd6826`.

## Preuves revues indépendamment

Documents récupérés directement sur GitHub : `DERNIER_RAPPORT.md`, `V2_1_FIXATIONS/DECISION.md`, `ASSEMBLAGE.md`, `APPROVISIONNEMENT.md`, `preuves/geometrie-et-limites.json`, `preuves/validation-finale.json`, métadonnées des deux commits. Les fichiers STEP/STL et scripts ont été construits et validés **selon les preuves Codex** ; ChatGPT n'a pas lui-même réexécuté CadQuery/OCCT ni inspecté physiquement l'assemblage. Le commit source ajoute les livrables V2.1 sans modifier les fichiers du PCB, schéma ou CAM existants ; Codex rapporte 464 fichiers historiques inchangés et relecture distante de 493 fichiers. Ces relectures exhaustives sont des déclarations Codex avec manifeste, pas une nouvelle comparaison exhaustive réalisée ici.

## Résultat technique accepté dans son périmètre

- **Solution candidate D126** : trois demi-couronnes isolantes à ouverture orientée, hauteur 12,25±0,10mm, diamètre enveloppe6mm, alésage2,4mm, avec trois vis nylon M2×20, deux écrous nylon par vis, têtes sous Nucleo. Pour deux prototypes : six supports + deux de réserve, vis8, écrous16 avec marges.
- L'analyse considère désormais un **flottement latéral possible de 1,015mm** autour des fixations : c'est une amélioration réelle du calcul par rapport à un appui supposé fixe. Le calcul aboutit à une garde **0,320338mm** contre une garde exigée 0,20mm, **uniquement sous budgets et géométries d'acceptation explicités**, non garantis pour les pièces physiques.
- Les 9 scénarios géométriques étudiés (3 trous × nominal/±5°) ont zéro collision avec 1060 solides du STEP ST ; ce n'est **pas** un balayage exhaustif des translations, rotations, tolérances, vibrations. Les faibles recouvrements nommés tête/PCB, profondeur ~4,42nm, sont documentés comme contacts numériques intentionnels et non effacés arbitrairement.
- Le filet nylon et l'épaisseur utile sous deux écrous ne suffisent pas à établir la tenue au serrage et à la vibration. Le calcul5N donne des contraintes moyennes, **pas une force ou un couple admissible qualifié**. Profil ouvert orienté±5° sans antirotation positive démontrée, fluage PETG, précision impression et rigidité demeurent des risques opérationnels.
- Alignement XY ST V1.9 et scénario Z/G V2.0 maintenus conditionnels pour l'assemblage physique. **Aucune seconde impression n'est requise par principe.**

## Coûts et approvisionnement

Le rapport donne chez SAM/ScrewsAndMore des lots de25 vis PA M2×20 à5,39€ et25 écrous PA M2 à5,39€, port14,90€, environ **25,68€ affichés / 25,90€ indicatifs TVA France**. Il n'y a pas de devis France ferme, de date de réception garantie ni de chiffrage du prestataire/temps d'impression des huit supports : total rendu `null`. **Ce panier ne doit pas être retenu sans comparaison au regroupement TME/DigiKey ou à la disponibilité locale**, au vu de la proportion des frais de port. Pas de substitution par vis métallique près des isolations ni de montage destructif.

## Verdict indépendant

**V2.1 étude géométrique : CONFORME À LA MISSION SUR L'ANALYSE ET LES LIVRABLES.** **Fixation mécaniquement/industriellement qualifiée : NON.** **Approvisionnement complet prix+délai : NON.** **PCB V1.9/Gerber : NON LIBÉRÉS, aucune commande autorisée par cette revue.**

Le profil D126 est **candidat de premier prototype**, pas fixation finale certifiée. Réserve principale désormais : vérifier l'orientation et le maintien de la pièce sous efforts/serrage, les dimensions effectivement réalisables, et le choix/achat proportionnés de la visserie avec G réel et engagement J5. La forme du coffret, vitrages et accès USER sont ajustables si le PCB reste inchangé. Une voie pragmatique serait de décider explicitement d'une **première fabrication de cinq PCB nus comme lot expérimental à risque contrôlé**, avec contrôle du premier montage à froid ; cela n'équivaut pas à une autorisation de commande et nécessite une décision financière humaine spécifique après transparence sur les coûts et risques résiduels.

Aucune nouvelle mission, opération physique, flash/debug ni achat déclenchés par la présente revue.
