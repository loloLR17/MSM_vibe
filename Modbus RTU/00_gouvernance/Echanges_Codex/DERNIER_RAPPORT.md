---
mission_id: TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN
status: IN_PROGRESS
base_ref: main
base_sha: 631505d0d6c466443c49b732795e56e804c6f502
initial_head: c2bb310b85e974a87048093575eb69afe0728264
result_sha: null
created_at_utc: 2026-11-06T10:22:24Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.7 — ABS, USER captif et dossier unique pour Morvan

**La conception et les contrôles numériques sont terminés :25 STL /61 pièces pour deux prototypes, avec vue éclatée complète et un seul ZIP « POUR MORVAN ».** IN_PROGRESS porte uniquement sur le commit/push puis la relecture distante encore à effectuer. Revue indépendante ChatGPT avant transmission, montage à froid indispensable ; aucune qualification physique revendiquée. Le précédent feu vert d'impression V2.6 a été retiré par la revue indépendante, conservée dans son archive. Le contrat USER V27 antérieur est annulé avant exécution ; seul le présent contrat a été exécuté.

Livrables : `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_7_KISS_ABS/`. Modifications ciblées de la mécanique, aucun changement de PCB/Gerber V2.3, firmware, électronique, positions ST, Zio/J5 ou G12,25. **842 fichiers historiques du shield comparés sans différence.** Aucun achat, impression, envoi à Morvan, manipulation, flash/debug ni modification de gouvernance.

ZIP unique : `pack_Morvan/TR2_V27_ABS_POUR_MORVAN_2_PROTOTYPES.zip`, **38 entrées /4014416 octets**, SHA-256 `7f21033ffeb50732b3223037159ed1aecf0052235d98e4e7c62ca02c599bbb8d`.25 STL, vues PNG/PDF, instructions, quantités/BOM, STEP éclaté et manifeste interne CRC/SHA ; réouverture intégrale locale réussie.

## Résultat et simplification

| Pour deux TR2 | Quantité |
|---|---:|
| V2.6 retirée |142 pièces imprimées|
| V2.7 complète |**61 pièces,25 types**|
| Assemblage utile |46 pièces,23 par prototype|
| Variantes non simultanées |12 :8 selles +4 tiges USER|
| Réserves |2 D126|
| Coupon ABS |1|
| Réduction |**81 pièces /57,04 %**|

Les60 petites cales imprimées disparaissent. FRAM/SD ont chacune une selle monobloc à deux portées :H10,6/11,2/11,8,1/2/3 creux d'identification, ponçage à plat≤0,6. Plage mécanique10..11,8 ; **pas une fenêtre électrique d'emboîtement**. H11,2 dans les STEP est un exemple, pas une mesure. Montage manuel des prises jusqu'à leur arrêt naturel puis choix de selle, sans fléchir les PCB ni serrer pour emboîter.

L'isolation reste explicite :**24 rondelles nylon standard DIN433 M2 2,2×4,5×0,3** distinctes pour deux TR2,16 sous les têtes M2×30 et8 au-dessus des modules Adafruit. Les ajouts standard remplacent les anciennes rondelles imprimées et ne sont pas masqués dans le compte. Ajile nylon acquis exclusivement D126 :6 vis/12 écrous utiles,8/16 avec réserves. M2×30 métal :16 ; écrous M2 :32. M3×12 métal :40 ; DIN934 M3 :30 ; DIN985 M3 :10. Deux ressorts courants d'exemple D-064 ou équivalents contrôlés,4 Colson/garnitures et2 vitres. Réserves optionnelles détaillées dans la BOM ; aucun achat effectué.

## USER sur ARNACO

B1 aligné aux coordonnées ST `(29,5212 ;120,3262)`, RESET distinct àX84,2074. TêteØ11 en retrait0,6 dans passageØ13, colleretteØ23 captive ; cage fixée sous le capot avec deux M3×12, tigeØ4/guideØ4,8 sur18mm. Ressort capturé autour de la tige entre deux sièges ; butée M3 orientée vers le haut avec écrou frein captif. Deux colonnettes imprimées assurent la retenue anti-chute de secours, **pas la limitation de course d'usage de B1**. Ni pièce libre en service, ni trappe/outil pour appuyer. Relief ARNACO conservé exactement au-dessus deZ50.

Trois tiges Z8,8/9,6/10,4, une choisie puis pointe ajustable hors carte. Dessus B1 à10,2 rapporté et premier clic≈0,4 sont des observations, pas une force/surcourse constructeur. La butée offre mécaniquement0,2..1,4 ; l'assemblage est montré au réglage court0,2, volontairement non opérationnel. **À froid :jeu positif au repos, réglage progressif jusqu'au vrai premier clic puis butée immédiate, retour franc vérifié. Ne jamais adopter aveuglément la course maximale.** Exemple calculé jeu0,3 +clic0,4 =course0,7, pas une consigne universelle.

Exemple de ressort DIN2098 D-064 :extérieur6,8±0,25, fil0,5, libre13,5±0,75, raideur0,728N/mm ; installé9..7,6, supérieur au minimum constructeur3,34. Force calculée du ressort3,28..4,30N, **pas une mesure de force de doigt ou une preuve de force B1 admissible**. Retour réel/friction et ergonomie gantée restent à vérifier. Un équivalent courant est accepté après contrôle des dimensions et du rappel ; aucun fournisseur imposé.

## Coffret, supports et accès

Trappe supérieure RS485 supprimée et rebouchée, vis associées retirées. Cassettes sud/ouest et portes USER/SD supprimées ; CN15 et poche microSD intégrés à la caisse. Deux petits panneaux spécifiques conservés, nord et est : une retouche d'ouverture ne demande pas de réimprimer toute la caisse. Base/capot158×166, corps fermé64mm/65 avec relief ; aucun plateau ou temps d'impression promis.

Nappe :panneau nord posé d'abord, IDC déjà assemblé couché dans le U supérieur, fermeture par le capot ; aucune fiche forcée dans la fente. RS485 :conducteurs raccordés capot ouvert, câble protégé posé sur la languette de base puis panneau est descendu autour du câble. PassageØ9 contrôlé avec enveloppeØ8 câble+protection, bride/Colson ; ce sont des scénarios de câble admissible, pas des dimensions physiques mesurées. Poche de doigt SD fixe, USB CN1/CN15, bornier et brides conservés. Maintenance du bornier par dépose du capot entier.

Guides/patins ABS5/5,6 (jeu0,3 par face), poches écrous AF6,1, coupelles isolantes1,5. Contact Click déplacé de0,2 vers le PCB nu :marge de bord nominale0,49 et0,29 après allocation relative±0,2, au lieu0,19 V2.6. Marge à premier composant natif≈1,495. Contacts MKI aux extrémités courtes :marges après allocation≈0,389/0,438. Cette allocation est un scénario de tolérance, pas une précision FDM qualifiée. Click natif1.01 et utilisateur1.02 **non déclarés identiques**, portées nues à confirmer à froid. Aucun pont métallique ajouté à l'isolation Click.

Têtes M2 descendues avec rondelles standard0,3 :Zmin9,75 nominal contre source STmax8,559996 aux zones examinées (marge1,19 nominale, pas garantie absolue du montage réel). Enveloppes R/C élargies d'après fiches et centres des vrais pads ; C2 D6/H14 de calcul laisse0,22 à l'extrémité pleine de prise SD (corps fabricant D5/H11 :0,72 nominal). Vérifier la pièce/inclinaison réelle ; aucune substitution électronique adoptée.

## Fichiers et contrôles réellement exécutés

- `QUANTITES.csv`, `BOM_QUINCAILLERIE.csv`, `IMPRIMEUR.md`, `MONTAGE_A_FROID.md`, `SOURCES.md` ; un seul paquet V2.7 neuf, anciens dossiers préservés.
- `vues/VUE_ECLATEE_V27.png`, vues fermé/implantation, vraie coupe BREP USER, `PLAN_MONTAGE_V27.pdf` de6 pages. **Un seul prototype,12 groupes numérotés,141 objets logiques**, Nucleo/shield/FRAM/SD/MKI/Click et visserie principale inclus.23 pièces imprimées montées, aucune variante/réserve/coupon assemblée simultanément.
- STEP éditables individuel, complet et éclaté, paramètres/générateurs Python. Enveloppes fabricants fidèles et citées ; aucun solide externe exact redistribué. Câbles/vis/ressort représentés par enveloppes de conception explicitement nommées. Fichiers STEP réouverts par CadQuery/OCC, **pas de session FreeCAD démontrée**.

| Validation | Résultat réel |
|---|---|
| `sources/modele.py` |25 STL/STEP ;21 nouveaux,4 formes V2.5 reprises octet pour octet ;23 instances utiles|
| `sources/controle.py` |**5477 contrôles BREP**, zéro intersection positive indésirable finale (seuil numérique1e-5mm³) ; modules natifs Click163/SD6, ST1060 solides via boîtes conservatrices, GerberST/EagleFRAM, visserie, caisse, USER et passages|
| Maillages |21 nouveaux réouverts fermés/orientés/un composant, STEP1 solide valide, erreur volume<1 % ;4 preuves V2.5 réutilisées avec identité exacte|
| USER |Captivité haut/bas, guidage/tilt théorique2,545°, butée/cage, deux scénarios B1 calibrés aux positions repos/courses0/0,2/0,7, plage ressort et absence RESET aligné|
| Appuis/patins |H10/10,6/11,2/11,8 pour les appuis, trois dessusPCB et extrêmes M3 ; passages/filetages/contacts dans les scénarios contrôlés|
| `sources/supplement.py` |**229 contrôles supplémentaires**, zéro intersection positive ; vrais centres pads et enveloppes R/C élargies, nouvelles pièces inchangées|
| LED / capot |**66/66** anciennes visées communes encore libres contre les nouvelles formes ; pas photométrie. Ancienne trappeRS rebouchée ; différence volumique ARNACO au-dessus Z50 =0|
| `sources/qualite_livraison.py` |Quatre STEP réimportés :23 solides propres,80 gabarits quincaillerie/patins,336 complet et336 éclaté ; tous les solides valides.25 SHA STL inchangés,61/46 quantités, PDF6 pages sans débordement de texte|
| Conservation |**842 fichiers antérieurs** comparés SHA-256 sans modification ; ni matériel ni tests firmware rejoués|
| Paquet |Tous les membres relus octet par octet ; CRC et manifeste SHA-256 complets ; preuve `preuves/paquet.json`|

Environnement existant :CadQuery2.8/Trimesh/Shapely/Matplotlib/PyMuPDF/VTK, Python `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`, `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`. Générateurs et sorties publiés ; caches fabricant natifs épinglés conservés dans `/home/lolo/dev/msm/reports/TR2-shield-v26-20261105/`, non inclus dans le paquet imprimeur. Aucun téléchargement commercial, test STM32, ERC/DRC ou cross-build répété sur électronique inchangée :ils n'ajoutent pas de preuve pertinente à cette mission mécanique.

**Échecs et corrections conservés** :premier modèle de cage refusé car colonnettes déconnectées, réunies par bras. Premier contrôle refusé sur19 intersections (16 sorties de guides partiellement rebouchées après élargissement ABS,3 contacts ressort/cage). Sorties recoupées jusqu'aux nouvelles faces extérieures, sièges et butée séparés ; aucun contrôle affaibli. Preuve `controle-01-refuse.json` et journaux réels. Éclaté de lecture corrigé car une projection mplot3d agrandie rognait le capot et le fond ; projection orthographique complète puis rendu VTK avec tampon de profondeur des vraies facettes pour supprimer les occultations incorrectes du tri de triangles ; vues finales inspectées. Aucune forme STL retouchée pour cette mise en page. Notice corrigée pour distinguer les deux D126 de réserve des pièces courantes. Le premier mode PDF seul refusait une table de formes non initialisée ; il recharge désormais les trois STEP concernés, sans relancer les validations de géométrie. `pdf-final-02.log` et `pdf-final.json` confirment la dernière génération et son contrôle.

## Limites et suite

**Acquis numérique, pas qualification physique** :le dossier existe sans attendre les PCB. Avant première alimentation, montage à froid décrit pas à pas :préinsérer M2/rondelles, serrer Ajile avant modules qui masquent les écrous, prises emboîtées naturellement puis selles, contacts nus/isolement Click réel, USER réglé et captif, aucun corps libre, câbles non pincés, SD/USB/LED réels libres.

Non démontrés :retrait ABS sur l'imprimante Morvan, G mesuré, vraies hauteurs/engagement des connecteurs, têtes Ajile non mesurées, force/course maximale B1 et retour réel, ergonomie gantée, fluage/vibrations/chocs/température, photométrie, tenue réelle de traction et IP. La butée réglable est réalisée ; **sa calibration réelle reste obligatoire**, aucune pression forte ou course arbitraire autorisée. Toute interférence réelle impose un arrêt local, pas un forçage ou une modification silencieuse du PCB.

Instructions ABS uniquement (matière non conductrice), coupon unique, profil machine/filament, D126H12,15..12,35 après ébavurage, pas de retrait global arbitraire modifiant les axes ST. Les réglages sont localisés ; Morvan n'a pas à redessiner les STL. Aucune impression ni contact imprimeur effectué. **À transmettre à ChatGPT :revue indépendante du ZIP V2.7 complet et de ces réserves, décision de remise ensuite.**

## Git, journal et publication

Départ vérifié c2bb310b distant/local sur worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V27-KISS-ABS`, branche `codex/shield-v27-kiss-abs`. Racine historique `/home/lolo/dev/msm/projects/MSM_vibe` propre à82169af7 préservée, malgré sa divergence ancienne ; pas de pull/reset/rebase/merge dessus.118 worktrees recensés, aucun supprimé. Stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87` inchangés. GitHub main relu avant publication ; aucun commit V2.7 antérieur perdu/réappliqué.

`ETAT_COURANT_TR2.md` actualisé par ajout V2.7 :retrait du feu vert V2.6, nouvelle livraison ABS/USER/KISS, contrôles et réserves physiques. Archive de mission exacte créée ; aucun document humain de gouvernance ou de contrat modifié. Permissions exercées :CAO/docs/tests puis commits et push normal vers `HEAD:refs/heads/main` ; flash/debug interdits et non utilisés.

Le commit technique, l'empreinte du ZIP, le résultat du push et la preuve de lecture distante seront consignés après publication. La clôture DONE sera réservée à la livraison numérique effectivement publiée/relue, sans qualifier le matériel.
