# MSM TR2 — Évolution UX de la supervision LIVE

**Statut : cadrage fonctionnel évolutif validé par l'utilisateur**, 18 octobre 2026. Référence : `main`, bases S6-H et S7-K, démonstration physique R5-B2. Ce document ne qualifie ni le profil LIVE complet ni le produit industriel.

## 1. Principes de conception

Préserver l'architecture S7 (.NET / projections autoritatives / API Web / SQLite), les six vues de détail S7-I2, les règles de sécurité B5, l'identité durable `device_id`, les qualités/continuités d'observation et les acquis R5-B2. L'évolution est **incrémentale** : ne pas reconstruire la supervision ni fusionner artificiellement profil V1 / profil LiveDiagnostic partiel. Le dispositif de démonstration `/live.html` est un outil diagnostique tant que les vues opérationnelles ne sont pas raccordées au modèle LIVE.

Les choix de détail d'ergonomie recommandés peuvent être réalisés de manière autonome par ChatGPT/Codex conformément à la délégation utilisateur, **sans figer définitivement le design**. Une décision structurante, un nouveau seuil métier, une action à risque ou une évolution normative reste distincte.

## 2. Vue générale du parc — décisions utilisateur

- **Une ligne par capteur TR2**, et non une ligne agrégée par machine. Le libellé affiche l'équipement et le point d'installation ; plusieurs capteurs peuvent appartenir au même équipement. Associer durablement le capteur à son équipement/emplacement sans modifier `device_id` ni utiliser le port COM comme identité.
- Montrer côte à côte **RMS global et crête globale** en `mg`, la **sévérité B3 transmise**, l'état de communication, la validité/fraîcheur, la date ou l'âge de dernière observation. Communication, état acquisition, qualité de la mesure, sévérité et état B5 restent distincts.
- En cas de perte communication / indisponibilité : conserver la dernière valeur réellement connue **uniquement avec indication non ambiguë de son ancienneté et du statut périmé**, sans remplacer une absence par zéro ni faire passer une mesure ancienne pour actuelle.
- Fonctions approuvées : filtre `Attention uniquement`, filtre par équipement/local, recherche rapide (équipement, emplacement, identifiant), tri des colonnes (nom, RMS, crête, sévérité, fraîcheur). Cas inconnu ou non identifié visibles honnêtement.
- Le comptage `Attention` est un regroupement de présentation, non un indice universel de sévérité. Éviter de qualifier comme panne un capteur volontairement arrêté.

## 3. Fiche détaillée TR2 — navigation S7 conservée

**Six onglets existants :** Synthèse ; Vibrations ; État & configuration ; Commandes ; Campagnes ; Diagnostic.

### 3.1 Synthèse

Conserver identité (machine, point, `device_id`), état communication, acquisition, fraîcheur et qualité, sévérité B3, RMS/crête globales, tableau RMS/crêtes X/Y/Z et dépassements signalés. Ajouter une **petite tendance récente**, alimentée uniquement par les observations historiques réellement disponibles et accompagnée de la qualité/provenance ; pas de fausses courbes.

### 3.2 Vibrations

- Courbe globale RMS / crête, affichage individuel des séries.
- Courbes temporelles X/Y/Z **superposées**, axes affichables/masquables séparément ; bascule **RMS ou crête** pour préserver la lisibilité.
- Périodes souhaitées : récente / 1h / 24h / 7j / plage personnalisée, **seulement si l'API et la rétention réelle fournissent ces données**. Si non disponible, signaler l'absence de couverture plutôt que créer des points interpolés.
- Représenter lacunes, interruptions, segments, valeurs invalides ou HELD et données périmées honnêtement ; jamais interpoler une perte ou réutiliser un ancien point en tant que mesure courante.
- Afficher, quand réellement documentés, des **seuils B4 compatibles en grandeur/unité et applicables à la configuration active**, et des **événements d'alarme effectivement observés** avec leur provenance temporelle explicite. Si seule l'heure de réception PC est connue, la marquer comme telle et ne jamais prétendre disposer de l'UTC de fenêtre. Si ambiguïté sémantique, omettre l'overlay et afficher une limite factuelle.
- Autoriser l'export CSV des observations effectivement sélectionnées, avec horodatage d'origine, qualité, identité, unité et provenance ; conserver lacunes et absence d'UTC, ne pas exporter un signal reconstruit comme brut.

### 3.3 État & configuration

Conserver sous-sections distinctes B1/B2/B4, configuration préparée vs active, lecture seule ; les informations absentes du profil partiel LIVE restent **indisponibles** et non rétrodéduites. Les décisions d'édition/changement de configuration ne sont pas introduites ici.

### 3.4 Commandes

Conserver **l'autorité unique B5** et tous les garde-fous S7 : `Prepared`, `Submitted`, `Ambiguous`, preuves terminales distinctes de l'acceptation HTTP ; aucune commande automatique, aucun retry/ignore/force, pas d'acquittement global, acquittement uniquement ciblé, confirmation pour reset. Ne pas greffer d'actions B5 au diagnostic `--live-direct` readonly.

### 3.5 Campagnes

Conserver le modèle documentaire/inventaire B6 de S7 et l'interface historique ; **la supervision LIVE n'attend pas la microSD**. Aucun téléchargement, lecture de bulk, stockage fictif ou état B6 inventé. Les fonctionnalités de stockage durable seront reprises dans R7 séparément.

### 3.6 Diagnostic

Séparer statut capteur (B1/B7 quand accessible), transport RS-485/PC, validité B3, erreurs du service, archivage SQLite et provenance des horodatages. Identités run/epoch/config absentes du fil LIVE diagnostique restent inconnues. Montrer l'état réel sans score santé ni inférence abusive.

## 4. Priorités d'implémentation recommandées

1. **Tranche UX-L1 :** raccorder une *vue par capteur* et une fiche Synthèse/Vibrations LIVE **en lecture seule** au vrai modèle R5 (`live_observations`, `/api/v2/live`), avec parcours historique borné réutilisant SQLite et sans doubles sources d'autorité. Ne pas altérer le `/api/v1/fleet` V1 ni les commandes S7. Identifier/qualifier clairement les fonctionnalités non disponibles du profil `LiveDiagnostic`. Incrément limité, logiciel PC seulement.
2. **UX-L2 :** enrichissement historique/graphique multi-axes, navigation temporelle, filtres et tris, export CSV, tests d'absence/qualité, toujours depuis les observations réellement persistées ; à décider selon les capacités existantes et le retour de L1.
3. **UX-L3 :** overlays de seuils/événements et finitions ergonomiques, seulement après preuve de leurs sources et de leur signification. Aucune nouvelle norme métrologique.
4. **R6 et R7** restent les autorités des qualifications physiques/firmware LIVE complet et du stockage bulk respectivement.

## 5. Critères de confiance et vérification

- UI/API sans contact Modbus direct ; aucune duplication de runtime ou écriture cachée.
- Démontrer la chaîne **SQLite réel / API / DOM** sur observations archivées et captures R5-B2, sans qualifier un replay comme acquisition courante.
- Contrôler sur les cas observés : mesure fraîche V/F, HELD après STOP, COM loss `Snapshot:null`, UTC nulle, segment et réception PC, identité inconnue, données absentes, données historiques et politique de fraîcheur.
- Tests existants proportionnés aux modifications ; préserver les 561 tests R5-B2 et les validations S7 sans exiger un nouveau passage matériel.
- Les données historiques R5-B2 et l'état matériel M016/FRAM final `2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84` sont **des références de preuve**, non des invitations à redémarrer/écrire la cible.
