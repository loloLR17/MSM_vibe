# PC machine — composantes spectrales PK : résultats et portée

**Six heures traitées ; résultats candidats ; aucun grade BV attribué.**

La voie étudiée mesure la crête de composantes individuelles : accélération de
1 à 5 Hz et vitesse de 5 à 100 Hz. La vitesse est obtenue à la fréquence de la
composante, sans convertir les anciens RMS. Les axes et les hypothèses de cadence
MCU/PC sont conservés séparément.

| Candidats MCU : maximum par minute sur XYZ | Médiane | P95 | Maximum |
|---|---:|---:|---:|
| Accélération 1–5 Hz, mm/s² crête | 4,8 | 7,8 | 11,5 |
| Vitesse 5–100 Hz, mm/s crête | 0,5 | 0,8 | 2,8 |

Ces statistiques incluent des cas non résolus et **ne classent pas le local**.
Les valeurs non arrondies, leurs fréquences et leurs réserves restent exportées.

Les méthodes spectrale et temporelle divergent jusqu'à 31 % sur certaines plages
d'accélération ; aucune fenêtre-axe de cette bande n'est jugée recevable pour
comparaison. En vitesse, 11 fenêtres-axes par hypothèse ont une composante quasi
stationnaire compatible, située sous le premier repère contextuel dans cette
seule portée. Aucune minute n'est recevable sur les trois axes : **la comparaison
globale de la paire reste indéterminée**.

Les deux graphiques montrent les candidats et leurs fréquences sans attribuer
les pics à une machine. Les repères BV concernent un PC machine occupé en
permanence à la mer, correspondance à confirmer. Les tableaux 5 et 6 de la section 3
portent sur des crêtes ; le tableau 4 des RMS pondérés décrit une autre grandeur.
La comparaison partielle en vitesse est explicitement indicative et non normative.

La fixation au double-face, la sensibilité et les axes du prototype ne sont pas
étalonnés ; UTC inconnue et deux bases de temps non qualifiées. Le fichier source
est inchangé et son intégrité interne contrôlée. La référence RAM indépendante
perdue après coupure n'est pas réhabilitée.

Les 30 804 résultats de fenêtres sont conservés en CSV et dans une copie migrée
SQLite v3, avec les 2 204 RMS précédents intacts. Les catalogues originaux et les
diagnostics antérieurs restent disponibles. Aucun nouveau test physique.

La suite utile est une revue indépendante des divergences et un cadrage de
l'extraction répétitive des composantes modulées avant toute lecture PK globale.
Le maximum spectral sur une courte fenêtre ne garantit pas le maximum répétitif
d'un signal modulé. La voie ISO/Wm reste un chantier séparé. Aucun effet sur la
santé, dommage structurel, causalité machine ou budget n'est déduit.

[Page illustrée](index.html) · [Accélération](BV_PK_ACC_1_5.png) · [Vitesse](BV_PK_VEL_5_100.png)
