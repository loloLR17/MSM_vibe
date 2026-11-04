---
mission_id: TR2-20261104-SHIELD-V17-VITRAGE-USER
status: READY
base_ref: main
base_sha: 4717839a68016bb840598bd37180106e978e2ae3
created_at_utc: 2026-11-04T15:40:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V1.7 : qualification ciblée vitrage d'état côté ST-LINK et poussoir USER

## Observations réelles et objectif

L'utilisateur a testé les LED sur **TR2-001** et confirme qu'il peut distinguer les trois LED LD1/LD2/LD3 depuis le **côté ST-LINK, avec une direction d'observation presque perpendiculaire au plan de la Nucleo**. Il propose d'étudier une **ouverture vitrée de ce côté du boîtier** afin de conserver les trois LED d'origine, sans électronique supplémentaire. Ce test se rapporte à la Nucleo réelle, **pas encore à l'empilage avec shield, modules, câble et boîtier**. Ne pas confondre la direction d'observation décrite avec une preuve d'aptitude d'une paroi latérale : établir géométriquement si une vitre en paroi ST-LINK ou en partie supérieure proche de cette face est mieux placée.

Cote opérateur acquise : **sommet bleu du bouton USER B1 au repos à 10,2 mm du dessus du PCB Nucleo**. Les cotes mâles CN12 8,22/2,40 mm et MKI 15,2 mm sont déjà connues. La **course réelle du bouton** n'a pas encore été communiquée ; ne pas présumer une valeur. L'utilisateur peut mesurer si c'est utile, mais il ne faut pas exiger une mesure dangereuse de butée/force : rechercher d'abord les caractéristiques constructeur et prévoir un poussoir captif à pointe légèrement compliante et butée prise sur la structure, avec marge au repos.

V1.6 étude LED est `DONE` pour le dossier documentaire, clôturée administrativement sous `archives/TR2-20261104-SHIELD-V16-ETUDE-LED/CLOTURE_CHATGPT_APRES_REVUE.md`, sans qualification optique ni libération fabrication. Lire `AGENTS.md`, gouvernance et protocole, rapport V1.6 et revue ChatGPT. Réutiliser `docs/ETUDE_LED_V1_6/`, `mechanical/V1_5/` et `cad/TR2_Shield_Proto_V1.kicad_pcb`, branch main.

## Travail demandé à Codex

1. **Vérifier l'option vitrage proche ST-LINK en priorité.** Mettre en regard l'observation utilisateur et les vues/coordonnées C05 réellement disponibles : trajectoire vers une vitre fermée côté ST-LINK, avec le shield 90×100, le MKIGIBV2, sa retenue, la nappe et le port ST-LINK pris en compte. Calculer ou illustrer les rayons depuis LD1/LD2/LD3 vers une **zone d'observation pratique**, en précisant clairement quels centres/hauteurs restent estimés et les marges. Comparer sans surdimensionnement avec la fenêtre nord-est de couvercle V1.6. Recommander la **plus simple solution ayant des dégagements raisonnables**, ou conclure explicitement que l'observation sans shield ne suffit pas. Ne pas inventer des cotes photogrammétriques précises ou considérer 0,246mm de marge V1.6 comme robuste. Si la paroi ST-LINK n'est pas orientée correctement pour la visée perpendiculaire, le signaler et proposer une autre position de fenêtre proche.
2. **Fenêtre complète** : plaque PC/PMMA transparent rapportée, cadre démontable et joint simple, absence de grande ouverture libre, sans prétendre à une classe IP. Préserver l'accès ST-LINK, la nappe et les câbles. Limiter le nombre de pièces et ne pas faire de guides/fibres coûteux pour deux prototypes. En cas d'impossibilité démontrée avec les dimensions disponibles, conserver le plan B trois LED reportées, sans l'exécuter automatiquement.
3. **USER** : intégrer la cote réelle10,2mm dans la chaîne de dimensions Nucleo→boîtier→poussoir ; chercher si la référence B1 permet de connaître la course/courbe d'effort nominale. Conserver une **course libre au repos**, une butée structurelle et une pointe souple/compliante pour ne jamais serrer sur le contact. Dimensionner un poussoir **réglable/ajustable** pour un prototype si l'on ne peut garantir la course nominale. Indiquer une seule éventuelle mesure complémentaire utile (course jusqu'à activation, pas course destructrice), et la méthode non agressive. RESET reste inaccessible en fonctionnement normal.
4. **Livrer des éléments exploitables** : courte note de décision et de réserves, coupe/vue 3D fondée sur la V1.5, emplacement candidat fenêtre et position/couloir USER, modèle mécanique éditable si cotes suffisantes, et checklist de validation physique ciblée. Faire apparaître quelles modifications du shield seraient réellement nécessaires **avant commande**, et quelles découpes du boîtier peuvent attendre. Ne pas produire de « vitrage validé » ou « bouton fonctionnel » sans preuves de compatibilité avec les dimensions réelles et tolérances.
5. **Ne pas modifier la CAO électronique/PCB/Gerber ni le firmware** pendant cette tranche d'étude. Aucun achat/impression, aucune soudure, aucune manipulation STM32. Publier les documents et vérifications sur main, puis `DERNIER_RAPPORT.md` avec `mission_id: TR2-20261104-SHIELD-V17-VITRAGE-USER`, statut fidèle, commits, preuves et relecture distante. Viser une fin de conception précommande sans nouvelles formalités inutiles.

## Autorisations et garde-fous

allow_commit true ; allow_push true ; allow_flash false ; allow_debug false. Circuit ChatGPT → GitHub → Codex → GitHub → ChatGPT. Respecter les autorisations de développement définies dans AGENTS.md, ne pas rouvrir les travaux fermés sans preuve, conserver les archives/campagnes et NE PAS COMMANDER les Gerber tant que géométrie et fonctionnalité ne sont pas qualifiées.
