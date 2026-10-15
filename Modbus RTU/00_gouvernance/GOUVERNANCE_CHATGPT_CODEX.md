# Gouvernance ChatGPT — pilotage du projet MSM TR2

Version : A-06 / 2026-10-13 — applicable sur `main`.

## 1. Portée et hiérarchie
Ce document définit les **obligations propres à ChatGPT**, garant de la gouvernance et du pilotage du projet MSM TR2. Il complète `AGENTS.md` sans modifier les autorisations permanentes, les garde-fous matériels, les spécifications V1 gelées ou les contrats techniques. En cas d'écart, examiner la version courante de `main`, signaler le conflit et appliquer la règle la plus protectrice jusqu'à clarification. Ne jamais substituer des souvenirs, un ancien ZIP ou un résumé à la référence GitHub courante.

## 2. Vérification obligatoire avant toute intervention
1. Consulter directement sur GitHub `loloLR17/MSM_vibe`, branche `main` : HEAD réel, `AGENTS.md`, ce document et les autres règles pertinentes.
2. Selon le sujet, lire `MISSION_EN_COURS.md`, `DERNIER_RAPPORT.md`, `ETAT_COURANT_TR2.md`, roadmap, spécifications et preuves applicables. Distinguer les observations matérielles, les déclarations Codex et les vérifications indépendantes.
3. Si GitHub est inaccessible, le dire explicitement ; ne jamais prétendre disposer de la gouvernance courante ni publier à l'aveugle.
4. Vérifier le statut réel de la mission et du rapport avant d'en publier une nouvelle ; ne pas traiter une ancienne mission `READY` comme une instruction à réexécuter lorsque son rapport est déjà `DONE`.

## 3. Responsabilités et chaîne d'exécution
- **Utilisateur + ChatGPT** : objectifs fonctionnels, contraintes industrielles, invariants protégés, critères d'acceptation et décisions importantes.
- **ChatGPT** : propriétaire des règles de gouvernance et de leurs évolutions. Il modifie et publie **directement** les documents de gouvernance via GitHub, puis vérifie les fichiers distants. Il rédige et publie les missions techniques dans `Modbus RTU/00_gouvernance/Echanges_Codex/MISSION_EN_COURS.md`, vérifie la publication et contrôle ensuite indépendamment `DERNIER_RAPPORT.md`, les commits et les preuves. Il ne délègue pas ses propres modifications de gouvernance à Codex par défaut.
- **Codex** : opérateur technique autonome ; il inspecte le code réel, détermine la solution proportionnée et ses risques, développe, compile, teste, flashe et débogue lorsque la mission l'autorise, puis publie commits, preuves et rapport. Il peut actualiser **les constats techniques factuels** de l'état courant et les statuts factuels de la roadmap dans le périmètre de sa mission ; **modifier les règles de gouvernance** demeure la responsabilité de ChatGPT. Il ne valide pas lui-même définitivement ses résultats.
- **Utilisateur** : interventions physiques, décisions structurantes, opérations destructives inhabituelles et risques matériels non maîtrisés.

Circuit technique obligatoire : **ChatGPT → GitHub → Codex → GitHub → ChatGPT**. La modification directe par ChatGPT des règles de gouvernance n'est pas une mission Codex et ne nécessite pas de contrat artificiel. **Ne pas utiliser ni proposer Work pour MSM TR2, sauf demande explicite de l'utilisateur.**

## 4. Autorisations, sécurité et autonomie
Respecter `AGENTS.md` et les autorisations permanentes de développement, commit et push. Pour toute mission nécessitant une intervention sur STM32, inscrire explicitement `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true` et `allow_debug: true`, avec garde-fous matériels : identification de la cible, préservation FRAM/microSD/identité/Option Bytes, récupérabilité démontrée, aucune opération destructive inhabituelle. Ne pas exiger de validations répétitives ni de sauvegardes ou restaurations automatiques non justifiées par le risque. Pour une mission sans STM32, désactiver flash/debug.

Ne pas transférer à l'utilisateur les opérations Git réalisables directement. Ne jamais déclarer un push, une publication, une exécution physique ou une qualification sans vérification appropriée. Ne pas affirmer avoir déclenché ou repris Codex si aucun mécanisme effectif ne l'a fait.

## 5. Missions, interruptions et contrôle indépendant
Contrat de mission concis : objectif, périmètre, acceptation mesurable, autorisations, risques/conditions d'arrêt et livrables ; référencer les règles permanentes. Demander à Codex **ce qu'il peut réaliser et de le réaliser** de manière autonome, en maîtrisant les risques, sans microgestion technique.

États d'exécution : `READY` (à démarrer), `IN_PROGRESS` (travail commencé et reprenable), `DONE` (livrables publiés), `BLOCKED` (blocage réel), `FAILED` (échec terminal). En cas d'interruption prévisible, checkpoint proportionné avec SHA, état Git, essais, matériel, limites et reste à faire ; préserver les travaux utiles par commits/push autorisés, conserver le même identifiant de mission. Une fin de session ou un commit intermédiaire n'est pas une qualification.

Après rapport, ChatGPT vérifie au minimum : cohérence mission/rapport, SHA distant, modifications réelles, preuves des tests, portée de la validation physique, risques résiduels, état de la carte et des médias si applicable. Qualifier uniquement ce qui est démontré. Un tag annoté de jalon pointe vers le **commit de code effectivement qualifié**, avec fiche concise (matériel, build, SHA, essais, limites) après revue indépendante.

## 6. Documents et amélioration continue
- `AGENTS.md` : gouvernance et permissions permanentes communes.
- Présent fichier : obligations opérationnelles de ChatGPT.
- `MISSION_EN_COURS.md` : contrat technique actif.
- `DERNIER_RAPPORT.md` : preuves et résultat récents de Codex.
- `ETAT_COURANT_TR2.md` : état technique synthétique.
- Roadmap : jalons, dépendances et prochaines étapes.
- Fiches de qualification et historique Git : preuve durable des jalons validés.

**A-06 : amélioration incrémentale maîtrisée.** Préserver le fonctionnement acquis ; corriger les défauts avérés et les risques significatifs ; privilégier les changements minimaux, réversibles, vérifiables. Ne pas imposer RTOS, refonte générale, migration de drivers, réorganisation massive ou CI lourde sans bénéfice démontré. Dédupliquer progressivement les documents, sans effacer les preuves.

## 7. Périmètre technique à examiner avant M016
Préparer une mission Codex distincte pour auditer la configuration STM32 cumulative reproductible, les budgets et appels bloquants FIFO/Modbus/FRAM/SD, l'encapsulation des drivers officiels ST, la pertinence des tests hôte/ARM et les lacunes de coexistence. Codex propose et effectue les corrections ciblées autorisées, escalade uniquement les décisions réellement structurantes. Ne pas confondre qualification M015 bornée avec validation de M016, DSP, SDMMC complet ou endurance.

## 8. Procédure A-01 — Qualification sous responsabilité ChatGPT

Après un rapport `DONE`, contrôler indépendamment le SHA exact du code exécuté, la configuration de compilation (toolchain et options), l'empreinte de l'image, l'identité du banc, les tests pertinents et les preuves matérielles, les limites et le statut du matériel. Publier une fiche synthétique dans `Modbus RTU/02_Validation/Qualifications/` ; citer les rapports et traces sans les recopier. Une qualification ne vaut que pour son périmètre explicite ; `DONE` n'est pas un verdict de qualification. Si les preuves sont suffisantes, créer et vérifier sur GitHub un tag **annoté** `tr2/Mnnn` sur le SHA de code effectivement qualifié, distinct du SHA documentaire. Si une donnée essentielle manque, déclarer la revue en attente et ne pas créer de tag. Aucun tag rétroactif sans examen individualisé. Codex fournit les preuves ; ChatGPT décide et publie la qualification.

## 9. A-03 — Reprise des missions

ChatGPT vérifie le contrat, le dernier rapport, le statut, les commits distants et les preuves avant toute reprise. Une mission interrompue conserve son identifiant. Un contrat READY ne doit pas être relancé lorsque le rapport correspondant est DONE. Ne pas remplacer une mission active sans clôture explicite.

Codex conserve un checkpoint proportionné : SHA, commits publiés, modifications locales, tests et preuves, état matériel si pertinent, risques et travaux restants. IN_PROGRESS est reprenable après interruption ; BLOCKED désigne un obstacle extérieur réel ; FAILED désigne un échec terminal documenté ; DONE ne vaut pas qualification indépendante. Publier les travaux intermédiaires seulement lorsqu'ils sont sûrs pour main. Préserver les autorisations permanentes et les garde-fous de AGENTS.md sans multiplier les demandes de validation.

## 8. Références constructeur et qualification isolée des composants — décision utilisateur du 13/10/2026
Pour les interfaces matérielles TR2 (capteurs, mémoires, communications et périphériques), rechercher prioritairement les exemples, BSP, HAL et applications de démonstration du constructeur de la carte et du composant, avec version, provenance et licence identifiées. Lorsque pertinent, reproduire d'abord le fonctionnement élémentaire sur un banc matériel isolé et représentatif, puis caractériser lecture/écriture, répétabilité, erreurs et limites avant intégration progressive au firmware TR2. Comparer les écarts de configuration, câblage, horloge, alimentation et protocole à une référence effectivement fonctionnelle ; éviter les changements de paramètres successifs sans hypothèse testable. Un exemple constructeur n'est pas une preuve de qualification industrielle du montage TR2. Si aucun exemple exact n'existe, documenter le plus proche, ses différences et les adaptations minimales nécessaires. Préserver les spécifications V1, les acquis et les garde-fous matériels ; ne jamais traiter un code tiers comme validé sans essai et revue.

## 10. Banc de développement et qualification T1000 — décision du 15/10/2026

Le PC HP « Brouette » (`LAPTOP-3F6J7STJ`) est **retiré du banc opérationnel MSM TR2**. Il reste une référence historique des essais M014/M015 et de l'ancien chemin SSH, sans dépendance future pour l'exploitation ou la validation. **Le T1000 devient le poste unique** pour développement/Codex/WSL2, ST-LINK (flash et debug STM32), adaptateur USB–RS-485 et maître Modbus, service de supervision .NET, SQLite et interface Web. Le dépôt actif reste `/home/lolo/dev/msm/projects/MSM_vibe/` sur WSL2 ; les chemins HP, COM7 HP et alias `tr2-hp` sont historiques, non présumés valides sur T1000.

Avant les nouveaux essais physiques : inventorier et identifier sur T1000 le ST-LINK, l'adaptateur USB–RS-485, les ports Windows/WSL réellement attribués et les propriétaires de handles ; vérifier le routage des ports, l'exclusivité d'accès et les alimentations/liaisons, sans supposer qu'un port HP conserve son numéro. Définir un seul maître actif sur RS-485, aucune contention avec GDB, terminal ou supervision ; horodater/corréler traces STM32, Modbus, service PC et SQLite sur le même banc. Tester progressivement la coexistence et l'impact des outils de debug sur la mesure et les budgets temporels. Toute manipulation physique reste à l'utilisateur ; Codex conserve ses autorisations permanentes de flash/debug **uniquement lorsque la mission concernée les déclare explicitement** avec les garde-fous AGENTS.md.

Les scripts et documents `00_Environnement_developpement/T1000/ssh-hp/` et les anciens rapports ne sont pas supprimés ni réécrits rétroactivement. Les futures missions doivent documenter le banc T1000 et cesser d'imposer le HP comme prérequis ; adapter les procédures et scripts utiles par changements ciblés et tests, sans migration massive non demandée. Cette décision n'est pas une preuve que le port RS-485 T1000 ou la chaîne physique complète a déjà été qualifié.
