# Arbitrage utilisateur — simplicité du bouton USER (04/11/2026)

Décision exprimée par le responsable du prototype TR2 à l'issue de la revue indépendante V1.7 :

> Le bouton doit être **très simple** ; ne pas lui consacrer un temps de conception disproportionné, ni faire exploser le prix ou la complexité du montage.

## Conséquences obligatoires pour la conception suivante

- **Priorité à un actionneur mécanique minimal**, imprimable avec le boîtier ou constituant une seule petite pièce rapportée, utilisant si possible le rappel propre du bouton B1. Pas de cage complexe, double guide, ressort réglé ou sous-assemblage multipièce sauf nécessité **démontrée** pour protéger B1.
- Conserver le **capuchon USER existant**, et les données physiques acquises : sommet au repos à **10,2 mm** depuis la face supérieure du PCB Nucleo, premier clic après **environ 0,4 mm**. La course admissible totale, la force maximale et les tolérances du vrai B1 ne sont **pas** établies ; ne jamais appliquer une précharge permanente ni compter sur un enfoncement forcé.
- Prévoir un **jeu au repos**, un axe correctement aligné, un actionnement manuel sans coincement et une limitation d'effort/course **simple et vérifiable** par la paroi du boîtier, avec pointe légèrement souple si nécessaire. Ne pas transformer la course de clic 0,4 mm en butée rigide supposée sûre.
- **Distinguer indispensable et facultatif** : si l'accès direct au bouton par une petite ouverture ou un bouton simple retenu par lèvre/collerette protège correctement contre les contacts involontaires, considérer cette variante avant le système V1.7 à ressort et réglage axial.
- **Aucun impact sur le shield/PCB, aucune électronique ajoutée et pas de nouveau composant coûteux** pour la seule fonction USER, sauf impossibilité technique explicitement prouvée. Pour les deux prototypes, minimiser coût, impression, assemblage, entretien.
- Ne pas relancer une mission d'étude autonome uniquement pour le poussoir : intégrer cet arbitrage à la **revue précommande / finalisation du coffret** avec les derniers contrôles géométriques, en respectant les autres gates C05/J5/Zio, LED, alimentation et ports.
- Le modèle `mechanical/V1_7/REVIEW_ONLY/user-principle.step` demeure une **option d'étude, pas un choix validé**. Aucune modification immédiate du rapport V1.7 ni libération fabrication par le présent arbitrage.

Objectif : fiabilité et proportionnalité industrielle, conformément au principe A-06 d'amélioration incrémentale maîtrisée.
