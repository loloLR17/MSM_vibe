# R5-B — preuves du checkpoint logiciel et du blocage Windows

Code évalué `dc773733a98d78016e1d0f8a341b155e1851e2c7`. Aucun essai physique LIVE R5-B, aucune ouverture COM4, aucun flash/reset/écriture FRAM. Préflight GDB lecture seule frais : Flash/FRAM complètes conformes R5AFIX et capteur arrêté. Les BIN/ELF R5B sont préparés, jamais flashés ; model-final.bin est un modèle hôte, jamais à restaurer sur cible.

561 tests .NET / sept scénarios JS simulés passent. Le service Linux avec chemin série volontairement inexistant sert HTTP200 et archive uniquement CommunicationLost/snapshot=null ; ce smoke n'est pas une acquisition. Le lancement Windows sur COM99 est refusé par Code Integrity avant la moindre ouverture série. Événements3033/3077 et erreur0x800711C7 conservés, aucune règle modifiée. Les images, sauvegardes, manifestes et sorties effectives sont contrôlables via BUNDLE.json et SHA256SUMS.

Les sources sous hardware contiennent des préparatifs hérités de restauration, NON exécutés sauf preflight.py/hardware.py ; elles ne constituent pas un contrôleur R5B qualifié. La reprise doit reprendre le plan LIVE_R5B.md après résolution autorisée du déploiement Windows. Ne jamais lancer d'ancienne recette R5AFIX avec les identités consommées5/10–5/11 ni charger le modèle FRAM.
