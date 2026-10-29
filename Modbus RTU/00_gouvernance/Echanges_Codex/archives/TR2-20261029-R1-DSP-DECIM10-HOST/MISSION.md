---
mission_id: TR2-20261029-R1-DSP-DECIM10-HOST
status: READY
base_ref: main
base_sha: a94fa2a87de7d93220e3e18f2dbd2f41c412bb83
created_at_utc: 2026-10-29T11:40:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — R1-DSP-A — Décimation /10 XYZ, oracle C et validation hôte/ARM sans matériel

## Contexte et autorité

La mission précédente `TR2-20261028-ACCEL-FS8G-VIBRATION-AC` est **administrativement sortie du pilote actif sans qualification** : copie intégrale et relue des documents dans `Echanges_Codex/archives/TR2-20261028-ACCEL-FS8G-VIBRATION-AC/{MISSION,RAPPORT}.md`, et son rapport final de l'investigation reste **`BLOCKED`** faute de signature Windows approuvée. **Ne pas interpréter la présence temporaire du rapport AC1 dans `DERNIER_RAPPORT.md` comme un résultat de la présente mission** : seul le rapport publié par Codex avec le nouvel identifiant en sera le résultat. Le dernier HEAD `main` est à revérifier avant toute écriture.

La **décision utilisateur du 29/10** confère à Codex l'autonomie permanente de packaging/signature/déploiement Windows, dans le cadre de `AGENTS.md`, mais **aucun certificat de confiance n'a été détecté** ; le problème Windows n'est pas un prérequis de cette mission. Il ne faut ni reprendre AC1 ni toucher aux anciens bundles ou au service Windows. Références de gouvernance : `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, [roadmap R0](../R0_METIER_20261029_ROADMAP.md).

## But de la tranche indépendante

Implémenter et tester **une brique DSP C légère, portable, purement logicielle** réalisant une **décimation causale /10 sur XYZ signés**, nominalement `80000/3 Hz -> 8000/3 ≈ 2666,667 Hz`, avec **FIR 107 coefficients** et maintien intégral du DC. Ce choix de travail R1 est préféré à l'ancienne cascade A225+B69/2 kHz pour simplicité, **sans dégeler les spécifications V1 historiques**. La brique doit être **réutilisable** dans la composition FIFO réelle sans créer maintenant un second lecteur capteur, une voie de stockage, un service Windows ou un changement de mapping Modbus gelé.

Sources obligatoires de paramètres, oracles et tests :
- [profil /10, numériques et limites](../R0_METIER_20261029_COMPARATIF_2667HZ_MODBUS_RTU.md), **§5** ;
- [plan R1-DSP, profil prioritaire §0](../R0_METIER_20261029_PLAN_ESSAIS_DETERMINISTES_DSP.md) ; les §§1–7 sont **historiques 2 kHz**, non des exigences opposables au /10 ;
- [revue FIFO, CPU/SRAM et étape R1-ARM](../R0_METIER_20261029_REVUE_R1_ARM_FIFO_CPU_RAM.md).

## Travaux autorisés et attendus

1. **Audit ciblé de la chaîne existante.** Identifier les types X/Y/Z, voies d'horodatage/index/callback FIFO, options CMake/GCC et contraintes de build du STM32U575. Réutiliser l'architecture, les tests et les helpers déjà disponibles ; **ne pas modifier B3, AC1, B4/B5, les services PC, la persistance FRAM ni le stockage**. Éviter RTOS/DMA/refonte pour principe.

2. **Brique FIR décimation /10 à trois axes.** Préserver l'origine de phase globale et l'état entre blocs arbitraires, sans allocation dynamique en traitement ni `malloc` par chunk, sans travail DSP en ISR, et avec ordonnancement source cohérent. La bande utile est **0–700 Hz** avec DC conservé, entrée nom. 26,667 kHz ; le rejet de toute composante qui se replie **dans 0–700 Hz** doit satisfaire le critère proposé de **−60 dB amplitude**, et la bande de sortie 700–1333 Hz **n'est pas une bande garantie**. Utiliser des coefficients FIR générés de manière déterministe (`firwin(107, 1333.3333333333333, fs=80000/3, window=("kaiser", 7.85726))` ou la transcription exacte documentée). Comparer différentes arithmétiques faisables (`float32`, Q15/Q31) avec le coût/marge réels, pas uniquement supposés ; choisir la voie minimale donnant la précision voulue. Vérifier les flags/ABI FPU effectifs avant d'affirmer un gain de vitesse.

3. **Qualité et robustesse.** Distinguer saturation physique d'un échantillon source et **ringing FIR provoquant un dépassement de la sortie int16** ; limiter explicitement, incrémenter ou publier dans l'API de la brique un compteur/flag de clipping, sans wrap ou écrêtage silencieux. Définir un état de segment pour début/fin de capture, faute source, désynchronisation, redémarrage et horloge effective `f_source/10` non garantie exactement ; aucune fabrication d'échantillons manquants ni de queue convolutionnelle déclarée « mesure ». La politique exacte de sérialisation archive reste au lot stockage.

4. **Tests reproductibles indépendants des découpes.** Exécuter et publier les `D10-00..D10-11` pertinents à cette couche : zéros, constantes XYZ, impulsion, tons 2/5/25/100/333⅓/666⅔/700Hz, repliements 1600/1966⅔/2000/2666⅔/6000Hz et balayage dense/borne analytique selon faisabilité, ±1/rails, impulsions extrêmes, orientation axes, chunks `1,7,13,31,255,512,1024`, STOP à phase non nulle, redémarrage, séquences perdues et invalidité. Référencer les **trois témoins SHA256 du modèle float64 6s/16 000 XYZ** sans exiger une égalité bit à bit si C utilise d'autres arrondis/coefficients ; reporter les écarts, tolérances et raisons. Favoriser tests hôte C avec oracle Python autonome non circulaire. Conserver sorties et manifeste déterministes en preuves hachées dans le dépôt, sans dumps démesurés.

5. **Build et intégrité ARM sans cible.** Compiler les tests C et le composant portable ; réaliser un cross-build ARM contrôlé avec l'outillage existant si applicable sans modification risquée de profils produit, vérifier `compile_commands`, attributs ELF/soft-vs-hard ABI, MAP, taille SRAM supplémentaire et résultat de linkage ; ne jamais présenter le nombre de MAC idéal `856 000/s` comme une **mesure de cycles STM32**. Préparer si utile l'interface de comptage DWT pour une mission matérielle distincte, sans flash/debug dans ce lot.

6. **Préservation Git et suites.** Ne modifier que les sources/tests/outils/notes directement utiles au FIR. Conserver les travaux d'autres worktrees, la branche de sauvegarde `archive/tr2/point-retour-pre-spi2-20261029` pointant vers `a3f5ac33a728a5afd028192654c17e5b04ee8d7b`, et le service Windows opérationnel. Respecter le circuit ChatGPT → GitHub → Codex → GitHub → ChatGPT. `allow_commit: true`, `allow_push: true` sur `main` : publier autonomement les commits sûrs, puis relire les fichiers et SHA distants. **`allow_flash: false`, `allow_debug: false` parce que cette tranche est exclusivement hôte/cross-compilation, sans intervention sur le STM32.** La prochaine mission R1-ARM physique aura `allow_flash: true`, `allow_debug: true` avec garde-fous matériels.

## Critères de sortie

- Compilation et tests hôte verts ; tests nouveaux et anciens non régressés dans le périmètre ;
- sortie XYZ et nombre d'échantillons corrects, causalité, phase globale, DC, tolérances spectral/alias et comportement à l'écrêtage documentés et testés ; reprise/pertes explicites ;
- brique portable versionnée, vérifications croisées avec oracle indépendant, reproductibilité des paramètres/coefficient/témoins ;
- coûts théoriques séparés de toute affirmation de CPU physique, réserves mémoire réelles du build documentées et prochaine mesure matérielle clairement définie ;
- `DERNIER_RAPPORT.md` publié par **Codex** avec le **mission_id de la présente mission**, `DONE` seulement si toutes les portes logicielles de cette tranche passent, sinon `BLOCKED` ou `FAILED` avec preuves ; contrôle distant et revue indépendante ChatGPT restent obligatoires.

**Aucune action humaine requise pour les travaux logiciels ordinaires.** Si un périphérique physique est nécessaire malgré le périmètre, ne pas le manipuler sous ce contrat : documenter la dépendance dans le rapport pour la mission suivante.
