---
mission_id: TR2-20261106-SHIELD-V27-PACK-MORVAN-PDF
status: READY
base_ref: main
base_sha: cc941c9c90051ff0bee78f8caa2b0e1290513907
created_at_utc: 2026-11-06T11:42:00+01:00
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — Dernière passe : dossier d'impression simplifié à remettre à Morvan (PDF + STL seulement)

## Objet et référence gelée

Le dossier de conception **TR2 V2.7 KISS ABS** vient d'être audité indépendamment par ChatGPT et accepté pour **impression prototype**, dans `Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN/REVUE_CHATGPT_INDEPENDANTE.md`. La géométrie CAO et les STL sont maintenant **GELÉS**. L'utilisateur demande uniquement de transformer le paquet *destiné à Morvan, l'imprimeur* en un lot **immédiatement compréhensible pour un non-technicien**, lisible sur n'importe quel ordinateur via PDF, sans MD/CSV/JSON/BOM/STEP à déchiffrer.

**NE MODIFIER AUCUNE PIÈCE, COTE, STL, STEP, géométrie, firmware, PCB, électronique, nomenclature de fabrication ni les quantités**. Préserver et ne pas écraser l'ancien ZIP technique `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_7_KISS_ABS/pack_Morvan/TR2_V27_ABS_POUR_MORVAN_2_PROTOTYPES.zip` (SHA-256 original `7f21033ffeb50732b3223037159ed1aecf0052235d98e4e7c62ca02c599bbb8d`) et toutes les versions antérieures. Il s'agit d'une **révision de DIFFUSION DOCUMENTAIRE V2.7**, pas d'une V2.8 de mécanique.

Le fichier `V2_7_KISS_ABS/QUANTITES.csv` et `preuves/paquet.json` ont été relus sur `main` par ChatGPT : le ZIP technique V2.7 contient **38 entrées dont 25 fichiers STL distincts** (une géométrie par fichier), à imprimer au total **61 fois** pour les deux prototypes :46 pièces utiles (23 par proto),12 alternatives non montées simultanément (selles H10,6/11,2/11,8 et trois tiges USER),2 D126 de réserve et1 coupon ABS. L'utilisateur a cru voir « 50 STL différents » ; **ne pas valider cette affirmation**. Expliquer le différentiel **25 modèles différents ≠ 61 exemplaires imprimés ≠ 2×25 modèles**.

## Mission Codex : un seul ZIP minimaliste POUR MORVAN, sans changer les modèles

1. Dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_7_KISS_ABS/pack_Morvan/`, créer **une seconde archive distinctement nommée** (exemple `TR2_V27_ABS_MORVAN_IMPRESSION_SIMPLE_2_PROTOTYPES.zip`). La nouvelle archive contient **uniquement :**
   - dossier `STL/` avec les **25 STL d'origine**, copiés **octet pour octet** et conservant leurs noms, orientations et unités mm, pas un STL dupliqué pour chaque copie à imprimer ;
   - `01_LIRE_EN_PREMIER_MORVAN.pdf`, notice courte en français courant ;
   - `02_VUES_DES_PIECES.pdf`, quelques vues CAO réalistes, avec la vue éclatée, la vue du boîtier fermé et éventuellement celle ouverte, utiles à la reconnaissance visuelle. Reprendre les vues V2.7 sans les réinventer et les légender simplement.
   - **Rien d'autre dans le ZIP de diffusion** : pas de fichiers `.md`, `.csv`, `.json`, `.step`, manifestes techniques, nomenclature métallique ou fichiers sources. Les preuves/manifestes restent bien sûr **dans le dépôt en dehors du ZIP** pour audit.
2. La notice PDF `01` doit permettre à **Morvan, non associé à nos cinq jours d'étude**, de lancer les impressions sans question : première page avec but « imprimer seulement, aucune électronique à monter », matière **ABS non chargé**, **2 prototypes**, dimensions des grandes caisses158×166×61, contrôle plateau + bordure, couleurs au choix, échelle100%/mm ; **tableau lisible des 25 noms exacts STL et nombre de copies à fabriquer POUR LES DEUX PROTOTYPES ENSEMBLE**, sous-groupé utile/variantes/réserves/coupon, avec total vérifié **61**. Aucun « multiplier tout le lot par deux ». Expliquer en une phrase les repères1/2/3creux des hauteurs alternatives et tiges, les deux D126 en réserve, et que **Morvan peut remettre les pièces en vrac, sans les trier ni les assembler**, le propriétaire se charge de l'assemblage. Ne lui demander aucun achat de vis, rondelle, ressort ou câble.
3. Inclure uniquement les **consignes d'impression pertinentes et raisonnables**, reprises de `IMPRIMEUR.md` V2.7 (sans inventer des valeurs) : ABS, orientation existante, préférence profil ABS éprouvé/enceinte/adhésion, couche caisses0,15–0,20, petites pièces0,10–0,15, densité selon la pièce, éventuels supports locaux nettoyables, coupon préalable si le producteur le souhaite, nettoyage/ébavurage, attention aux D126 hauteur fonctionnelle12,15..12,35mm si mesurable. Ne pas forcer un contrôle avec les vis qu'il ne possède pas. Pas d'exigence de conditionnement/tri/étiquetage manuel qui le ferait travailler davantage. Être clair sur les aléas du retrait ABS grandes pièces et sur le fait qu'une intervention de reprise éventuelle sera faite par le propriétaire.
4. **La notice visuelle PDF `02`** doit comprendre des images qui montrent ce que les pièces deviennent assemblées, avec repères assez grands et légendes en français courant. Il ne faut pas transformer cela en guide de montage long : Morvan n'assemble pas les cartes, il peut seulement observer et éventuellement tester l'emboîtement de pièces plastiques évidentes, sans quincaillerie. **Ne pas demander à Morvan de vérifier le fonctionnement du bouton USER, le jeu des cartes ou le ressort**, ce sont des contrôles ultérieurs du propriétaire.
5. Garder les détails du fabricant et l'ancien `PLAN_MONTAGE_V27.pdf`, README/BOM/CSV/JSON/STEP complets et preuve techniques **disponibles dans le dépôt**, mais hors du ZIP diffusion. Il faut que les informations essentielles à Morvan soient conservées dans le(s) PDF compréhensibles ; ne pas envoyer un simple PDF renvoyant à des fichiers CSV/MD externes. Ne pas falsifier les quantités, le statut « imprimable prototype », ni la libération V2.7 déjà accordée.
6. Contrôler **automatiquement** que les25 STL sont **strictement identiques** aux STLs V2.7 source (taille/SHA-256) et qu'aucun STL n'est oublié/dupliqué ; comparer la table de quantités du PDF aux données source, total61 (46+12+2+1) ; ouvrir les PDF et vérifier text extraction, caractères français, accents, pagination, tables, et surtout **RENDRE LES PDF EN IMAGES et inspecter toutes les pages** pour éviter texte tronqué, chevauchements ou petits caractères illisibles ; ZIP relu intégralement avec CRC/empreinte, inventaire entrées (25 STL et2 PDF), nom distinct et SHA256 final. Ne pas prendre le succès de conversion Markdown→PDF comme preuve d'une notice lisible : produire une véritable mise en page accessible et visuelle.
7. Effectuer un commit/push normal sur `main`, relecture distante et rapport `DERNIER_RAPPORT.md` sous ce `mission_id` avec chemins/empreintes, comptage25/61, preuves de PDF visuels et limites. Conserver la V2.7 technique, tous les STL source et ancien ZIP strictement intacts. Aucune transmission automatique à Morvan : **ChatGPT décidera et remettra le lien de l'archive simplifiée après revue indépendante**.

## Critères d'acceptation et permissions

**Succès = un seul ZIP pour Morvan comprenant uniquement25 STL+2PDF lisibles, avec toutes quantités exactes et les formes gelées**, intuitif pour une personne qui n'a pas participé à la conception, sans tâche de montage ni tri imposée. **Aucun changement de mécanique et aucune nouvelle décision fonctionnelle** ; n'ouvrir aucune nouvelle mission de CAO. Les seuls travaux sont édition de notice, visualisation existante, script de conditionnement/audit et publication du nouveau paquet. `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Aucun achat, impression, envoi email, matériel ou flash/debug.
