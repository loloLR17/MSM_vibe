# Revue indépendante ChatGPT — V2.6 dossier complet, règle KISS et quantité de pièces

## RECTIFICATIF PRIORITAIRE — accès USER : retrait du GO impression (05/11/2026)

**STATUT COURANT : NO-GO pour transmettre/imprimer le ZIP V2.6 tel quel. Le « GO borné » mentionné plus bas est ANNULE.** Cette annotation prime sur la qualification précédente, conservée pour traçabilité. Signalement utilisateur après ouverture du modèle `02_couvercle_ARNACO` sous FreeCAD : aucune commande USER visible depuis la face supérieure. L'analyse CAO confirme que le USER B1 est accessible par une ouverture de la **cassette sud sous trappe pivotante M3**, sans bouton de commande extérieur. Le modèle de cette trappe n'exige pas le dévissage pour chaque activation, mais cela ne satisfait pas l'accès opérationnel attendu : pas de petit capot à ouvrir ni pièce pouvant se perdre dans les fonds, accès direct sans outil.

**Régression historique vérifiée dans GitHub :** `mechanical/V1_5/parameters.json`, `USER_principle`, proposait déjà un actionneur extérieur captif (tigeØ5, guidageØ5,5, têteØ10, butée captiveØ12, position de repos/limiteur de course et rappel à qualifier). Cette solution était alors un **principe non encore qualifié physiquement**. La mission V2.4 rédigée par ChatGPT a remplacé cette prescription par le critère vague « accès USER simple mais protégé ». Codex a effectivement implémenté la solution de trappe directe dans `V2_4_IMPRESSION_3D/sources/modele.py` (commit `eb322dec649c36645edbbf7d2881e95ee0ea2df8`, lignes 122–125). ChatGPT a accepté ce changement sans relever la perte de l'actionneur extérieur, puis repris sans correction en V2.5/V2.6. **Responsabilité du pilotage et de la libération : ChatGPT.** L'utilisateur n'a pas autorisé cette simplification fonctionnelle.

**Exigence désormais explicite :** commande USER extérieure directement accessible, protégée contre activation accidentelle, **une pression sans outil et sans ouvrir/devisser/retirer une trappe, capot ou pièce quelconque**, ensemble captif avec butée mécanique empêchant précharge/surcourse et pièce perdue, réponse raisonnablement robuste aux vibrations, RESET non actionnable par erreur. Recourir si possible à une solution simple ciblée sur la cassette sud, en préservant les autres pièces validées. Poursuivre l'objectif d'un unique dossier pour deux prototypes cette nuit, sans attendre les livraisons, mais **l'ancien ZIP n'est pas à transmettre à l'imprimeur**. Une nouvelle mission ChatGPT corrigera la CAO, les modèles, tests et le ZIP ; une nouvelle revue indépendante sera nécessaire avant feu vert.

---


Date : 2026-11-05. Référence `loloLR17/MSM_vibe/main`, HEAD initial de revue `b863ead63ca55858433e43746a820f3ef030a89a`.

## Contrat, commits et preuves publiées

Mission `TR2-20261105-SHIELD-V26-IMPRESSION-COMPLETE-SANS-RECEPTION` : `MISSION_EN_COURS.md` encore étiquetée `READY` historiquement, mais `DERNIER_RAPPORT.md` de même identifiant `DONE`. Ne pas relancer cette mission. Commit technique réellement évalué `348e559cef823a57a86a99a98d2ef2376a6c5f7d`, directement issu de l'addendum `72a28db9c96d35d7d421574f464cc0125c380a20` ; commit de clôture `b863ead63ca55858433e43746a820f3ef030a89a`, fils direct du commit technique. Chaîne de parents vérifiée par ChatGPT sur GitHub.

La mission demandait **un unique ZIP imprimable pour deux prototypes avant réception des PCB**, sans prétendre à une qualification physique, avec supports FRAM/SD ajustables et quatre étriers MKI/Click inclus. Le livrable numérique répond à ce contrat : `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_6_IMPRESSION_COMPLETE/pack_imprimeur/TR2_V26_COMPLET_2_PROTOTYPES.zip`. ZIP déclaré **32 entrées, 25 types STL, 142 unités physiques** ; SHA-256 déclaré `2fee5474fc251d96a5d2b01eb2df2c50561965d72085f32319f07780f9a80230`, 545997 octets.

## Recalcul indépendant des quantités et effet « puzzle »

Le fichier `QUANTITES.csv` et `preuves/pieces.json` relus directement depuis `main` donnent :
- Base V2.5 : **40** imprimées, dont **36 utilisées** (24 pièces uniques pour les deux boîtiers, 6 D126 et 6 bras Nucleo) et 4 réserves.
- Appuis FRAM/SD H8/H10/H12 : **24** imprimés, **8 choisis** selon hauteur réelle, 16 alternatives restantes.
- Cales0,5/1/2 mm : **12+36+12=60** imprimées, **24 rondelles 1mm intégrées au montage nominal**, les 36 autres pour calage facultatif/réserve ; 8 calages1mm étaient envisagés mais restent dépendants de la hauteur à froid.
- Étriers MKI+Click : **4** imprimés, 4 utilisés.
- Patins MKI/Click : **4+4+6=14** imprimés, **8 utilisés**, 6 réserves.

Contrôle : 40+24+60+4+14=**142**, soit **71 par prototype dans l'envoi à imprimer**. Assemblage nominal minimal : 36+8+24+4+8=**80 pièces pour deux prototypes**, soit **40 par prototype**, plus cales de réglage selon besoin. **62 unités sont des options, variantes ou réserves**, non 62 pièces de mécanisme supplémentaires à monter. Une forte majorité des unités en sus sont minuscules et à trier ; la *complexité logistique de leur impression et identification est réelle*, même si la masse CAO est faible. Ne pas imposer au fabricant amateur 71 opérations d'assemblage par boîtier.

## Contrôles indépendants réalisés et qualification bornée

ChatGPT a lu directement sur GitHub le rapport complet, la mission actuelle, `AGENTS.md`, la gouvernance, le README, `QUANTITES.csv`, `BOM_QUINCAILLERIE.csv`, `IMPRIMEUR.md`, `MONTAGE_A_FROID.md`, `SOURCES.md`, `preuves/synthese-controles.json`, `controle-final.log`, `contacts-et-bords.json`, `plages-visserie.json`, `relecture-maillages.json`, `relecture-assemblages.json`, `masse.json`, `pieces.json`, `sources-fabricants.json`, `revisions-photos-addendum.json`, `pack-manifeste.json`, `publication-initiale.json`, `conservation-finale.json`, et les métadonnées des deux commits. Les quantités et les statuts ci-dessus ont été recomptés indépendamment ; le nombre de fichiers et les enchaînements de commits sont cohérents.

Preuves exécutées **par Codex** (relues, pas réexécutées par ChatGPT) : 1097 vérifications BREP sans intersection positive indésirable, 11 nouveaux maillages contrôlés, 14 STL/STEP V2.5 repris identiques, modèles Click officiels163 solides/SD6, plans ST Gerber et Eagle Adafruit, 32 membres ZIP relus avec CRC/empreintes, 747 fichiers historiques conservés. Un recalcul autonome des BREP à partir des gros modèles officiels hors dépôt n'a pas été effectué. **Le hash SHA-256 sur les octets du ZIP n'a pas été recalculé indépendamment dans la présente revue** : l'archive binaire est visible par les commits et la documentation GitHub et a été relue par Codex, mais l'accès binaire direct par l'interface de ChatGPT n'a pas été obtenu. Les données CRC/hashes sont donc les preuves documentées par Codex, pas une deuxième exécution indépendante.

La CAO utilise la documentation primaire : ST Gerbers MKIGIBV2, Eagle FRAM/SD, Connfly8,5, STEP officiel Click`v101` (163 solides). La vraie carte Click utilisateur est marquée **HWREV1.02** : aucune équivalence intégrale1.01↔1.02 démontrée. La hauteur H naturelle n'a pas été mesurée ; appuis rigides8/10/12+calages et ponçage léger couvrent les variantes sans prétendre à un engagement électrique garanti. Marges résiduelles documentées étroites : contact Click après budgetXY±0,2 ≈0,19mm, appui Adafruit ≈0,14mm ; tête M2 vs composant ST ≈0,29mm dans le modèle. **Premier montage à froid impératif** : vérifier impression/têtes, contacts sur PCB nu, révision réelle, absence de flexion, isolement et ports ; adapter le PETG seulement si nécessaire, aucune mise sous tension avec doute.

La nomenclature standard ajoute des approvisionnements à vérifier selon stock personnel : **M3×12 métal 42 utiles (50 cible), M3 DIN934 34 (40 cible), M3 DIN985 8 (10 cible), M2×30 métal 16 (20 cible), M2 DIN934 32 (40 cible)** ; Ajile6vis+12écrous utilisés seulement pour D126 et stocks achetés8/16. Pas de nouvelle visserie nylon spécialisée. Les grandes cotes et accès boîtier restent V2.5. Prévoir un tri/sachetage des petites rondelles par taille ; attention aux cales0,5mm sur plateau PETG (première couche/adhésion) sans imposer de nouvelle création CAO.

## Décision ChatGPT et principe A-06/KISS

**GO borné pour envoyer à l'ami imprimeur le ZIP V2.6 comme un dossier complet d'impression de deux prototypes, avec ajustements à froid annoncés.** La mission numérique demandée est accomplie sans réception matérielle. **NO-GO pour annoncer un montage physique déjà qualifié, une tenue vibratoire/électrique/IP démontrée ou pour mettre sous tension sans premier contrôle.**

142 impressions paraît excessif, mais la décomposition montre qu'il s'agit de **25 géométries dont beaucoup de minuscules cales de prévision**, contre40 pièces nominalement montées par prototype. Le surcoût de pièces en nombre tient essentiellement à éviter une deuxième expédition/reimpression et à l'incertitude des hauteurs. Ne **pas déclencher une nouvelle itération de refonte ou un report** seulement pour réduire artificiellement ce chiffre, sans défaut significatif prouvé ; KISS privilégie ici un seul envoi et une notice claire. Une optimisation simple du nombre de cales ne sera envisagée que si le coût/temps/sortie du trancheur de l'imprimeur le justifie. La masse du scénario CAO est737,72g dense pour le lot, non une mesure tranchée ; aucun délai ou tarif d'impression garanti.

L'utilisateur reste responsable d'envoyer le fichier à l'ami imprimeur et des premiers contrôles physiques. Aucun fichier n'a été envoyé par ChatGPT/Codex, et aucun STL/ZIP n'a été modifié lors de cette revue.
