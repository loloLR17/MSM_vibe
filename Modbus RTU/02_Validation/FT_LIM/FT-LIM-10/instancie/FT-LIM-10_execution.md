# FT-LIM-10 — Procédure d’exécution

1. Lire un snapshot cohérent du Bloc 3.
2. Contrôler les domaines enum et les bits réservés.
3. Contrôler les registres réservés 3040..3047.
4. Relever les deux compteurs sur plusieurs snapshots sans reset/RAZ identifié et vérifier leur monotonie ; accepter la saturation à 0xFFFFFFFF.
5. Identifier la configuration active associée aux calculs observés.
6. Préparer, si l’environnement de test le permet, une configuration dont les seuils permettent de distinguer ancien et nouveau comportement, sans l’activer.
7. Vérifier qu’une simple modification préparée ne rétroagit pas sur les valeurs déjà calculées.
8. Activer la nouvelle configuration par le mécanisme validé FT-LIM-03/04.
9. Vérifier conditionnellement que les nouveaux seuils prennent effet à partir de la prochaine fenêtre de calcul validée.
10. En cas d’indisponibilité temporaire observable, accepter la conservation d’une ancienne valeur mais vérifier qu’elle est qualifiée par les mécanismes de validité/fraîcheur ; ne pas imposer un masque exact non spécifié.
11. Vérifier la convention d’unité par traçabilité du mapping V1.
12. Classer les relations non normées en TRACE_ONLY/NOT_DEFINED.

Les tests ne provoquent aucune écriture dans le Bloc 3, entièrement RO.
