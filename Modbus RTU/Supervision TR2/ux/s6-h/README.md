# S6-H — Maquette Web navigable

Cette maquette matérialise les décisions UX validées en S6-A à S6-G pour la supervision locale du projet MSM — Capteur de vibration TR2.

## Statut

- maquette UX pré-matériel ;
- données exclusivement de démonstration, non normatives ;
- aucune Web API ;
- aucun accès Modbus ;
- aucune commande réelle ;
- aucun framework Web imposé ;
- aucun impact sur la solution .NET existante.

## Utilisation

Ouvrir `index.html` dans un navigateur moderne. Pour éviter les restrictions éventuelles liées au chargement de fichiers locaux, on peut aussi servir ce répertoire avec n’importe quel serveur HTTP statique local.

## Parcours couverts

- Vue générale du parc ;
- comparaison vibration en accélération `mg` ;
- Attention & diagnostic avec séparation des sources ;
- inventaire campagnes B6 ;
- vues Système / Communications / Configuration PC ;
- détail TR2 avec Synthèse, Vibrations, État & configuration, Commandes, Campagnes, Diagnostic ;
- exemple de transaction B5 `Ambiguous` avec blocage des nouvelles commandes ;
- confirmation UX renforcée pour `SOFTWARE_RESET` sans émission réelle.

## Invariants de la maquette

- pas de vitesse `mm/s` en V1 ;
- pas de FFT, spectre, diagnostic vibratoire automatique ou seuil inventé ;
- dernière valeur connue clairement qualifiée lorsqu’elle n’est plus fraîche ;
- état TR2, communication PC et fraîcheur ne sont pas fusionnés ;
- `device_id` reste l’identité durable, le port COM n’est jamais une identité équipement ;
- une transaction B5 ambiguë n’offre ni `Réessayer`, ni `Ignorer`, ni `Forcer résolu` ;
- B6 est présenté comme inventaire de métadonnées, sans faux téléchargement de données brutes ;
- configuration PC affichée en lecture seule.

## Hors périmètre

Le branchement à un runtime réel, le choix d’un framework Web de production, l’authentification, les rôles, l’édition B4, l’import SD, les analyses FFT, le packaging et le Windows Service restent hors S6-H.
